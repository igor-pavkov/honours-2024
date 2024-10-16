static ut64 disarm_8bit_offset (ut64 pc, ut32 offs) {
        /* pc (really, op->addr) is 64 bits, so we need to sign-extend
         * to 64 bits instead of the 32 the actual CPU does */
        ut64 off = offs;
	/* sign extend if higher bit is 1 (0x08) */
	if ((off & 0x80) == 0x80)
	{
		off |= ~0xFF;
	}
	return (off<<1) + pc + 4;
}