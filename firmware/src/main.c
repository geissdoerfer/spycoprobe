#include "FreeRTOS.h"
#include "message_buffer.h"
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
#define PIN_PROG_ENABLE 6

#define PIN_LED0 12
#define PIN_LED1 13

static TaskHandle_t led_taskhandle, usb_taskhandle, cmd_taskhandle;
static MessageBufferHandle_t sbw_req_buf;

void led_thread(void *ptr) {
  do {
    gpio_put(PIN_LED0, true);
    gpio_put(PIN_LED1, false);

    vTaskDelay(pdMS_TO_TICKS(250));
    gpio_put(PIN_LED0, false);
    gpio_put(PIN_LED1, true);
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
  static char req_buf[64];
  uint32_t count;

  count = tud_cdc_n_read(itf, req_buf, sizeof(req_buf));
  xMessageBufferSend(sbw_req_buf, req_buf, count, portMAX_DELAY);
}

/* Processes requests and prepares response. Returns number of bytes in
 * response. */
int sbw_process(sbw_req_t *request, sbw_rsp_t *response) {
  int rc;

  switch (request->req_type) {
  case SBW_REQ_START:
    response->rc = sbw_dev_start();
    return 1;
  case SBW_REQ_STOP:
    response->rc = sbw_dev_stop();
    return 1;
  case SBW_REQ_HALT:
    response->rc = sbw_dev_halt();
    return 1;
  case SBW_REQ_RELEASE:
    response->rc = sbw_dev_release();
    return 1;
  case SBW_REQ_WRITE:
    response->rc =
        sbw_dev_mem_write(request->address, request->data, request->len);
    return 1;
  case SBW_REQ_READ:
    response->rc =
        sbw_dev_mem_read(response->data, request->address, request->len);
    response->len = request->len;
    return 2 + (response->len * 2);
  default:
    response->rc = SBW_RC_ERR_UNKNOWN_REQ;
    return 1;
  }
}

/* Processes commands received via USB */
void cmd_thread(void *ptr) {
  int rc;
  sbw_req_t request;
  sbw_rsp_t response;
  sbw_pins_t pins = {.sbw_tck = PIN_SBW_TCK,
                     .sbw_tdio = PIN_SBW_TDIO,
                     .sbw_dir = PIN_SBW_DIR,
                     .sbw_enable = PIN_PROG_ENABLE};

  sbw_dev_setup(&pins);

  do {
    /* Fetch command from buffer */
    xMessageBufferReceive(sbw_req_buf, &request, sizeof(sbw_req_t),
                          portMAX_DELAY);

    int rsp_len = sbw_process(&request, &response);

    /* Send the response packet */
    tud_cdc_n_write(0, &response, rsp_len);
    tud_cdc_n_write_flush(0);
  } while (1);
}

int main() {

  gpio_init(PIN_LED0);
  gpio_set_dir(PIN_LED0, true);
  gpio_init(PIN_LED1);
  gpio_set_dir(PIN_LED1, true);

  gpio_init(PIN_TARGET_POWER);
  gpio_set_dir(PIN_TARGET_POWER, true);
  gpio_put(PIN_TARGET_POWER, true);

  gpio_init(PIN_PROG_ENABLE);
  gpio_set_dir(PIN_PROG_ENABLE, true);

  stdio_init_all();
  board_init();

  tusb_init();

  sbw_req_buf = xMessageBufferCreate(256);

  xTaskCreate(usb_thread, "USB", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, &usb_taskhandle);

  xTaskCreate(led_thread, "LED", configMINIMAL_STACK_SIZE, NULL,
              tskIDLE_PRIORITY + 1, &led_taskhandle);

  xTaskCreate(cmd_thread, "CMD", 512, NULL, tskIDLE_PRIORITY + 1,
              &cmd_taskhandle);

  vTaskStartScheduler();

  return 0;
}
