#ifndef __SBW_DEVICE_H_
#define __SBW_DEVICE_H_

#include "sbw_jtag.h"
#include <stddef.h>
#include <stdint.h>

int sbw_dev_setup(sbw_pins_t *sbw_pins);
int sbw_dev_start(void);
int sbw_dev_stop(void);

int sbw_dev_get_coreip_id(uint16_t *coreip_id);
int sbw_dev_get_device_id(uint16_t *device_id_ptr);

int sbw_dev_mem_read(uint16_t *dst, uint32_t addr);
int sbw_dev_mem_write(uint32_t addr, uint16_t data);

int sbw_device_disable_mpu(void);

int sbw_dev_release(void);
int sbw_dev_halt(void);

int sbw_dev_pc_set(uint32_t addr);
int sbw_dev_reg_set(uint8_t reg, uint32_t data);
int sbw_dev_reg_get(void);
int sbw_dev_erase();

#endif /* __SBW_DEVICE_H_ */