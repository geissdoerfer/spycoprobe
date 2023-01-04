import serial
import numpy as np
import struct
from typing import Union

from spycoprobe.protocol import ReqType
from spycoprobe.protocol import ReturnCode


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

    def _recv_rsp(self):
        """Receive a packet from CDC ACM serial and decode it."""
        rsp = ""
        while len(rsp) == 0:
            rsp = self._ser.read_all()

        rc = int(rsp[0])
        if rc != ReturnCode.SBW_RC_OK:
            raise Exception(f"Probe replied with RC {rc}")

        if len(rsp) > 1:
            # Extract length of payload
            dlen = int(rsp[1])
            # Extract payload
            data = np.frombuffer(rsp[2:], dtype=np.uint16, count=(dlen // 2))
            return data

    def start(self):
        """Puts device under JTAG control."""
        pkt = struct.pack("=B", int(ReqType.SBW_REQ_START))
        self._ser.write(pkt)
        self._recv_rsp()

    def stop(self):
        """Releases device from JTAG control."""
        pkt = struct.pack("=B", int(ReqType.SBW_REQ_STOP))
        self._ser.write(pkt)
        self._recv_rsp()

    def halt(self):
        """Halt CPU execution."""
        pkt = struct.pack("=B", int(ReqType.SBW_REQ_HALT))
        self._ser.write(pkt)
        self._recv_rsp()

    def release(self):
        """Continue CPU execution."""
        pkt = struct.pack("=B", int(ReqType.SBW_REQ_RELEASE))
        self._ser.write(pkt)
        self._recv_rsp()

    def write_mem(self, addr, data: Union[int, np.ndarray]):
        """Write a word to NVM or RAM."""
        if hasattr(data, "__len__"):
            pkt = struct.pack(f"=BBI{len(data)}H", ReqType.SBW_REQ_WRITE, len(data), addr, data)
        else:
            pkt = struct.pack(f"=BBIH", ReqType.SBW_REQ_WRITE, 1, addr, data)
        self._ser.write(pkt)
        self._recv_rsp()

    def read_mem(self, addr, dlen: int = 1):
        """Read a word from NVM or RAM."""
        pkt = struct.pack(f"=BBI", ReqType.SBW_REQ_READ, dlen, addr)
        self._ser.write(pkt)
        return self._recv_rsp()
