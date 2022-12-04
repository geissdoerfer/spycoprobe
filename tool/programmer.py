import serial
from crccheck.crc import Crc16Ccitt
import struct
import crccheck
import binascii
from enum import IntEnum
from intelhex import IntelHex
from typing import Tuple
import numpy as np
import time
import click

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


class SpycoProbe(object):
    def __init__(self, port, baudrate=1000000, timeout=1.0):
        self._port = port
        self._baudrate = baudrate
        self._timeout = timeout

    def __enter__(self):
        self._ser = serial.Serial(self._port, self._baudrate, timeout=self._timeout)
        return self

    def __exit__(self, *exc):
        self._ser.close()

    def _send_cmd(self, cmd_type: CmdType, addr: int = 0x0, data: int = 0x0):
        pkt = struct.pack("=HBIB", data, int(cmd_type), addr, MARKER)
        self._ser.write(pkt)

    def _recv_rsp(self):
        rsp = self._ser.read(4)
        if len(rsp) != 4:
            raise Exception("No valid response received")
        data, rc, marker = struct.unpack("=HBB", rsp)
        if marker != 0xAA:
            raise Exception("Wrong marker")
        if rc != ReturnCode.RC_OK:
            raise Exception(f"Probe replied with RC {rc}")

        return data

    def start(self):
        self._send_cmd(CmdType.CMD_START)
        self._recv_rsp()

    def stop(self):
        self._send_cmd(CmdType.CMD_STOP)
        self._recv_rsp()

    def halt(self):
        self._send_cmd(CmdType.CMD_HALT)
        self._recv_rsp()

    def release(self):
        self._send_cmd(CmdType.CMD_RELEASE)
        self._recv_rsp()

    def write_mem(self, addr, value: int):
        self._send_cmd(CmdType.CMD_WRITE, addr, value)
        self._recv_rsp()

    def read_mem(self, addr):
        self._send_cmd(CmdType.CMD_READ, addr)
        return self._recv_rsp()

    def write_mem_block(self, addr, values: np.array):
        for i, value in enumerate(values):
            self._send_cmd(CmdType.CMD_WRITE, addr + i * 2, value)

        for i in range(len(values)):
            self._recv_rsp()

    def read_mem_block(self, addr, n_words):
        for i in range(n_words):
            self._send_cmd(CmdType.CMD_READ, addr + i * 2)

        values = np.empty((n_words,), dtype=np.uint16)
        for i in range(n_words):
            values[i] = self._recv_rsp()
        return values


if __name__ == "__main__":
    ih = IntelHex()
    ih.loadhex("/tmp/mspwhatever/_build/build.hex")
    data_dict = ih.todict()

    with SpycoProbe("/dev/ttyACM1") as probe:
        probe.start()
        probe.halt()
        probe.write_mem(0x1F00, 0xBEEF)
        readback = probe.read_mem(0x1F00)
        print(f"{readback:04X}")
        probe.write_mem(0x1F00, 0x0)
        readback = probe.read_mem(0x1F00)
        print(f"{readback:04X}")
        probe.write_mem_block(0x1F00, np.arange(16))
        readback = probe.read_mem_block(0x1F00, 16)
        print(readback)
        probe.release()
        probe.stop()
