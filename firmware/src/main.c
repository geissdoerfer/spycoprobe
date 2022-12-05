#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "pico/stdlib.h"
#include <stdio.h>

#include "bsp/board.h"
#include "tusb.h"

#include "protocol.h"
#include "sbw_device.h"

#define PIN_SBW_TCK 2
#define PIN_SBW_TDIO 3
#define PIN_SBW_DIR 4
#define PIN_TARGET_POWER 5

static TaskHandle_t led_taskhandle, usb_taskhandle, cmd_taskhandle;
static QueueHandle_t cmd_queue;

static char rx_buf[64];

void led_thread(void *ptr) {
  do {
    board_led_on();
    vTaskDelay(pdMS_TO_TICKS(250));
    board_led_off();
    vTaskDelay(pdMS_TO_TICKS(250));

  } while (1);
}

/* Processes all events from tinyUSB stack */
void usb_thread(void *ptr) {
  do {
    tud_task();
    vTaskDelay(1);
  } while (1);
}

/* This callback is invoked when the RX endpoint has data */
void tud_cdc_rx_cb(uint8_t itf) {
  uint32_t count;
  cdc_cmd_t *cmd;

  count = tud_cdc_n_read(itf, rx_buf, sizeof(rx_buf));
  cmd = (cdc_cmd_t *)rx_buf;

  if (cmd->marker != PKT_MARKER) {
    puts("CDC callback: wrong marker!");
    return;
  }

  if (count != sizeof(cdc_cmd_t)) {
    puts("CDC callback: wrong length!");
    return;
  }

  /* Put the decoded result into a queue for processing*/
  xQueueSend(cmd_queue, cmd, portMAX_DELAY);
}

/* Processes commands received via USB */
void cmd_thread(void *ptr) {
  int rc;
  cdc_cmd_t cmd;
  cdc_rsp_t response = {.marker = PKT_MARKER};
  sbw_pins_t pins = {
      .sbw_tck = PIN_SBW_TCK, .sbw_tdio = PIN_SBW_TDIO, .sbw_dir = PIN_SBW_DIR};
  sbw_dev_setup(&pins);

  do {
    /* Fetch command from queue */
    xQueueReceive(cmd_queue, &cmd, portMAX_DELAY);

    switch (cmd.cmd_type) {
    case CMD_START:
      rc = sbw_dev_start();
      break;
    case CMD_STOP:
      rc = sbw_dev_stop();
      break;
    case CMD_HALT:
      rc = sbw_dev_halt();
      break;
    case CMD_RELEASE:
      rc = sbw_dev_release();
      break;
    case CMD_WRITE:
      rc = sbw_dev_mem_write(cmd.address, cmd.data);
      break;
    case CMD_READ:
      rc = sbw_dev_mem_read(&response.data, cmd.address);
      break;
    default:
      rc = RC_ERR_UNKNOWN_CMD;
    }

    if (rc != SBW_ERR_NONE) {
      response.rc = RC_ERR_GENERIC;
    } else {
      response.rc = RC_OK;
    }

    /* Send the response packet */
    tud_cdc_n_write(0, &response, sizeof(cdc_rsp_t));
    tud_cdc_n_write_flush(0);
  } while (1);
}

int main() {
  stdio_init_all();
  board_init();

  tusb_init();

  cmd_queue = xQueueCreate(128, sizeof(cdc_cmd_t));

  xTaskCreate(usb_thread, "USB", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, &usb_taskhandle);

  xTaskCreate(led_thread, "LED", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, &led_taskhandle);

  xTaskCreate(cmd_thread, "CMD", 512, NULL, tskIDLE_PRIORITY + 1,
              &cmd_taskhandle);

  vTaskStartScheduler();

  return 0;
}
