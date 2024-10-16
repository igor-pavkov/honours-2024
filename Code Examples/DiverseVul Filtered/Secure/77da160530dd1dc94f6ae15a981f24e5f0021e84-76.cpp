static inline int major_to_index(unsigned major)
{
	return major % BLKDEV_MAJOR_HASH_SIZE;
}