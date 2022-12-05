from intelhex import IntelHex


class IntelHex16bitReader(IntelHex):
    def __init__(self, source=None):
        super().__init__(source)

    def iter_words(self):
        """Iterates segments and yields addresses and corresponding 16-bit values."""
        for addr, addr_stop in self.segments():
            while addr < addr_stop:
                value = self[addr] + (self[addr + 1] << 8)
                yield addr, value
                addr += 2
