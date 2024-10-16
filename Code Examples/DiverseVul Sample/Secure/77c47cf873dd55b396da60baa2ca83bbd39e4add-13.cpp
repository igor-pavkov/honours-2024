static ut64 disarm_12bit_offset (RAnalOp *op, unsigned int insoff) {
	ut64 off = insoff;
	/* sign extend if higher bit is 1 (0x0800) */
	if ((off & 0x0800) == 0x0800)
	{
		off |= ~0xFFF;
	}
	return (op->addr) + (off<<1) + 4;
}