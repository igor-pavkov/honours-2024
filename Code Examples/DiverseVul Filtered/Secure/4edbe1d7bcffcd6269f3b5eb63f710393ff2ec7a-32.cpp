static void init_buckets(struct list_head *buckets)
{
	unsigned int i;

	for (i = 0; i < NUM_BUCKETS; i++)
		INIT_LIST_HEAD(buckets + i);
}