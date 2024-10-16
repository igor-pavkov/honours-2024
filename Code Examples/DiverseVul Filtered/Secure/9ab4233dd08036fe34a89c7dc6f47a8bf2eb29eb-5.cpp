static int madvise_need_mmap_write(int behavior)
{
	switch (behavior) {
	case MADV_REMOVE:
	case MADV_WILLNEED:
	case MADV_DONTNEED:
		return 0;
	default:
		/* be safe, default to 1. list exceptions explicitly */
		return 1;
	}
}