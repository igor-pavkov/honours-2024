static u8 sinp(int offset)
{
	if (iommap)
		/* the register is memory-mapped */
		offset <<= ioshift;

	return inb(io + offset);
}