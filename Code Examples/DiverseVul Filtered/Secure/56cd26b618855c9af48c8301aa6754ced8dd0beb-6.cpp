static void soutp(int offset, u8 value)
{
	if (iommap)
		/* the register is memory-mapped */
		offset <<= ioshift;

	outb(value, io + offset);
}