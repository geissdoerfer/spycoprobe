#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <stdint.h>

#define PKT_MARKER 0xAA

enum {
  CMD_START,
  CMD_STOP,
  CMD_HALT,
  CMD_RELEASE,
  CMD_WRITE,
  CMD_READ,
};

enum { RC_OK, RC_ERR_GENERIC, RC_ERR_UNKNOWN_CMD };

/* Format of packets received from the host */
typedef struct __attribute__((packed)) {
  uint16_t data;
  uint8_t cmd_type;
  uint32_t address;
  uint8_t marker;
} cdc_cmd_t;

/* Format of packets sent to the host */
typedef struct __attribute__((packed)) {
  uint16_t data;
  uint8_t rc;
  uint8_t marker;
} cdc_rsp_t;

#endif /* __PROTOCOL_H_ */