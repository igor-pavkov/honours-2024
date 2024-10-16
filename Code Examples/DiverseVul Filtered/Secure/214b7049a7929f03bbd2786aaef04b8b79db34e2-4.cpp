static int __init dnotify_init(void)
{
	dn_cache = kmem_cache_create("dnotify_cache",
		sizeof(struct dnotify_struct), 0, SLAB_PANIC, NULL);
	return 0;
}