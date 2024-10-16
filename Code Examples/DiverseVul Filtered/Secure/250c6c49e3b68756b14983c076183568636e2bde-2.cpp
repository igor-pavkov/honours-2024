static unsigned long sbusfb_mmapsize(long size, unsigned long fbsize)
{
	if (size == SBUS_MMAP_EMPTY) return 0;
	if (size >= 0) return size;
	return fbsize * (-size);
}