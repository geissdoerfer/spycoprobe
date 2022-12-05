/*
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * This implementation of the SBW transport layer is based on code provided
 * by TI (slau320 and slaa754). It provides the basic routines to serialize
 * the JTAG TMS, TDO and TDI signals over a two wire interface.
 */

#include "sbw_transport.h"
#include "sbw_hal_glue.h"

static bool tclk_state = 0;
static unsigned int clk_delay_us;

static sbw_pins_t pins;

static inline void tmsh(void) {
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_HIGH);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);
}

static inline void tmsl(void) {
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);
}

static inline void tmsldh(void) {
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_HIGH);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);
}

static inline void tdih(void) {
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_HIGH);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);
}

static inline void tdil(void) {
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);
}

static inline bool tdo_rd(void) {
  bool res;

  hal_gpio_set(pins.sbw_dir, GPIO_STATE_LOW);
  hal_gpio_dir(pins.sbw_tdio, GPIO_DIR_IN);

  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  res = hal_gpio_get(pins.sbw_tdio);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_HIGH);

  hal_gpio_set(pins.sbw_dir, GPIO_STATE_HIGH);
  hal_gpio_dir(pins.sbw_tdio, GPIO_DIR_OUT);

  hal_delay_us(clk_delay_us);
  return res;
}

static inline void tdo_sbw(void) {
  hal_gpio_set(pins.sbw_dir, GPIO_STATE_LOW);
  hal_gpio_dir(pins.sbw_tdio, GPIO_DIR_IN);

  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_LOW);
  hal_delay_us(clk_delay_us);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_HIGH);

  hal_gpio_set(pins.sbw_dir, GPIO_STATE_HIGH);
  hal_gpio_dir(pins.sbw_tdio, GPIO_DIR_OUT);

  hal_delay_us(clk_delay_us);
}

void set_sbwtdio(bool state) { hal_gpio_set(pins.sbw_tdio, state); }

void set_sbwtck(bool state) { hal_gpio_set(pins.sbw_tck, state); }

void tmsl_tdil(void) {
  HAL_ENTER_CRITICAL();
  tmsl();
  tdil();
  tdo_sbw();
  HAL_EXIT_CRITICAL();
}

void tmsh_tdil(void) {
  HAL_ENTER_CRITICAL();

  tmsh();
  tdil();
  tdo_sbw();
  HAL_EXIT_CRITICAL();
}

void tmsl_tdih(void) {
  HAL_ENTER_CRITICAL();

  tmsl();
  tdih();
  tdo_sbw();
  HAL_EXIT_CRITICAL();
}

void tmsh_tdih(void) {
  HAL_ENTER_CRITICAL();

  tmsh();
  tdih();
  tdo_sbw();
  HAL_EXIT_CRITICAL();
}

bool tmsl_tdih_tdo_rd(void) {
  HAL_ENTER_CRITICAL();

  tmsl();
  tdih();
  bool res = tdo_rd();
  HAL_EXIT_CRITICAL();
  return res;
}

bool tmsl_tdil_tdo_rd(void) {
  HAL_ENTER_CRITICAL();

  tmsl();
  tdil();
  bool res = tdo_rd();
  HAL_EXIT_CRITICAL();
  return res;
}

bool tmsh_tdih_tdo_rd(void) {
  HAL_ENTER_CRITICAL();

  tmsh();
  tdih();
  bool res = tdo_rd();
  HAL_EXIT_CRITICAL();
  return res;
}

bool tmsh_tdil_tdo_rd(void) {
  HAL_ENTER_CRITICAL();

  tmsh();
  tdil();
  bool res = tdo_rd();
  HAL_EXIT_CRITICAL();
  return res;
}

void clr_tclk_sbw(void) {
  HAL_ENTER_CRITICAL();
  if (tclk_state == GPIO_STATE_HIGH) {
    tmsldh();
  } else {
    tmsl();
  }

  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_LOW);

  tdil();
  tdo_sbw();
  tclk_state = 0;
  HAL_EXIT_CRITICAL();
}

void set_tclk_sbw(void) {
  HAL_ENTER_CRITICAL();

  if (tclk_state == GPIO_STATE_HIGH) {
    tmsldh();
  } else {
    tmsl();
  }
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_HIGH);

  tdih();
  tdo_sbw();
  tclk_state = 1;
  HAL_EXIT_CRITICAL();
}

bool get_tclk(void) { return tclk_state; }

int sbw_transport_stop(void) {
  hal_gpio_set(pins.sbw_dir, GPIO_STATE_LOW);
  hal_gpio_dir(pins.sbw_tdio, GPIO_DIR_IN);
  hal_gpio_dir(pins.sbw_tck, GPIO_DIR_IN);

  hal_gpio_set(pins.target_power, GPIO_STATE_LOW);

  tclk_state = 0;
  return 0;
}

int sbw_transport_start(void) {

  hal_gpio_set(pins.target_power, GPIO_STATE_HIGH);

  hal_gpio_set(pins.sbw_dir, GPIO_STATE_HIGH);
  hal_gpio_dir(pins.sbw_tdio, GPIO_DIR_OUT);
  hal_gpio_set(pins.sbw_tdio, GPIO_STATE_HIGH);

  hal_gpio_dir(pins.sbw_tck, GPIO_DIR_OUT);
  hal_gpio_set(pins.sbw_tck, GPIO_STATE_HIGH);

  tclk_state = 0;
  return 0;
}

int sbw_transport_setup(sbw_pins_t *sbw_pins) {
  pins.sbw_tck = sbw_pins->sbw_tck;
  pins.sbw_tdio = sbw_pins->sbw_tdio;
  pins.sbw_dir = sbw_pins->sbw_dir;
  pins.target_power = sbw_pins->target_power;

  hal_gpio_init(pins.sbw_tdio);
  hal_gpio_init(pins.sbw_tck);
  hal_gpio_init(pins.sbw_dir);
  hal_gpio_init(pins.target_power);

  hal_gpio_dir(pins.target_power, GPIO_DIR_OUT);
  hal_gpio_set(pins.target_power, GPIO_STATE_LOW);

  hal_gpio_dir(pins.sbw_dir, GPIO_DIR_OUT);
  hal_gpio_set(pins.sbw_dir, GPIO_STATE_LOW);

  /*
   * Make sure that clock frequency is around 500k. This number is taken from
   * TI's slaa754 reference implementation and works reliably where other values
   * do not work. In SLAU320AJ section 2.2.3.1., the 'delay' is specified as 5
   * clock cycles at 18MHz, but this seems to not work reliably and contradicts
   * the reference implementation.
   */
  clk_delay_us = 3;

  return 0;
}
