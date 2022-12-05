import serial
import numpy as np
import struct

from spycoprobe.protocol import CmdType
from spycoprobe.protocol import ReturnCode
from spycoprobe.protocol import MARKER


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
        """Build a packet with defined format and send it over CDC ACM serial."""
        pkt = struct.pack("=HBIB", data, int(cmd_type), addr, MARKER)
        self._ser.write(pkt)

    def _recv_rsp(self):
        """Receive a packet from CDC ACM serial and decode it."""
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
        """Puts device under JTAG control."""
        self._send_cmd(CmdType.CMD_START)
        self._recv_rsp()

    def stop(self):
        """Releases device from JTAG control."""
        self._send_cmd(CmdType.CMD_STOP)
        self._recv_rsp()

    def halt(self):
        """Halt CPU execution."""
        self._send_cmd(CmdType.CMD_HALT)
        self._recv_rsp()

    def release(self):
        """Continue CPU execution."""
        self._send_cmd(CmdType.CMD_RELEASE)
        self._recv_rsp()

    def write_mem(self, addr, value: int):
        """Write a word to NVM or RAM."""
        self._send_cmd(CmdType.CMD_WRITE, addr, value)
        self._recv_rsp()

    def read_mem(self, addr):
        """Read a word from NVM or RAM."""
        self._send_cmd(CmdType.CMD_READ, addr)
        return self._recv_rsp()

    def write_mem_block(self, addr, values: np.array):
        """Send commands consecutively and collect responses afterwards."""
        for i, value in enumerate(values):
            self._send_cmd(CmdType.CMD_WRITE, addr + i * 2, value)

        for i in range(len(values)):
            self._recv_rsp()

    def read_mem_block(self, addr, n_words):
        """Send commands consecutively and collect responses afterwards."""

        for i in range(n_words):
            self._send_cmd(CmdType.CMD_READ, addr + i * 2)

        values = np.empty((n_words,), dtype=np.uint16)
        for i in range(n_words):
            values[i] = self._recv_rsp()
        return values
