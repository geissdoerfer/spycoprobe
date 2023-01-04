#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <stdint.h>

enum {
  SBW_REQ_START,
  SBW_REQ_STOP,
  SBW_REQ_HALT,
  SBW_REQ_RELEASE,
  SBW_REQ_WRITE,
  SBW_REQ_READ,
};

enum { SBW_RC_OK, SBW_RC_ERR_GENERIC, SBW_RC_ERR_UNKNOWN_REQ };

/* Format of packets received from the host */
typedef struct __attribute__((packed)) {
  uint8_t req_type;
  uint8_t len;
  uint32_t address;
  uint16_t data[29];
} sbw_req_t;

/* Format of packets sent to the host */
typedef struct __attribute__((packed)) {
  uint8_t rc;
  uint8_t len;
  uint16_t data[31];
} sbw_rsp_t;

#endif /* __PROTOCOL_H_ */