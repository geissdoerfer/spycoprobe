from enum import IntEnum

MARKER = 0xAA


class CmdType(IntEnum):
    CMD_START = 0
    CMD_STOP = 1
    CMD_HALT = 2
    CMD_RELEASE = 3
    CMD_WRITE = 4
    CMD_READ = 5


class ReturnCode(IntEnum):
    RC_OK = 0
    RC_ERR_GENERIC = 1
