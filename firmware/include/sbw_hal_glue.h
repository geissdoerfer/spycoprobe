#ifndef __HAL_H_
#define __HAL_H_

#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"

typedef enum { GPIO_DIR_OUT, GPIO_DIR_IN } gpio_dir_t;

typedef enum { GPIO_STATE_LOW, GPIO_STATE_HIGH } gpio_state_t;

static inline int hal_gpio_init(unsigned int pin) {
  gpio_init(pin);
  gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_2MA);
  return 0;
}

static inline int hal_gpio_dir(unsigned int pin, gpio_dir_t dir) {

  if (dir == GPIO_DIR_IN) {
    gpio_set_dir(pin, GPIO_IN);
  } else {
    gpio_set_dir(pin, GPIO_OUT);
  }
  return 0;
};

static inline int hal_gpio_cfg(unsigned int pin, gpio_dir_t dir,
                               gpio_state_t state) {
  gpio_put(pin, state);
  hal_gpio_dir(pin, dir);
  return 0;
}

static inline int hal_gpio_set(unsigned int pin, gpio_state_t state) {
  gpio_put(pin, state);
  return 0;
}

static inline gpio_state_t hal_gpio_get(unsigned int pin) {
  return gpio_get(pin) ? GPIO_STATE_HIGH : GPIO_STATE_LOW;
}

static inline void hal_delay_us(unsigned int time_us) { sleep_us(time_us); }

static inline void hal_delay_ms(unsigned int time_ms) { sleep_ms(time_ms); }

#define HAL_ENTER_CRITICAL taskENTER_CRITICAL
#define HAL_EXIT_CRITICAL taskEXIT_CRITICAL

#endif /* __HAL_H_ */