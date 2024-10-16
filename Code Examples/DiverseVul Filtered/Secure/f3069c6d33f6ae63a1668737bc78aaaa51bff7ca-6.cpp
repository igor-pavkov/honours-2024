static unsigned int rds_pages_in_vec(struct rds_iovec *vec)
{
	if ((vec->addr + vec->bytes <= vec->addr) ||
	    (vec->bytes > (u64)UINT_MAX))
		return 0;

	return ((vec->addr + vec->bytes + PAGE_SIZE - 1) >> PAGE_SHIFT) -
		(vec->addr >> PAGE_SHIFT);
}