static int __init aio_setup(void)
{
	kiocb_cachep = KMEM_CACHE(kiocb, SLAB_HWCACHE_ALIGN|SLAB_PANIC);
	kioctx_cachep = KMEM_CACHE(kioctx,SLAB_HWCACHE_ALIGN|SLAB_PANIC);

	aio_wq = create_workqueue("aio");
	abe_pool = mempool_create_kmalloc_pool(1, sizeof(struct aio_batch_entry));
	BUG_ON(!abe_pool);

	pr_debug("aio_setup: sizeof(struct page) = %d\n", (int)sizeof(struct page));

	return 0;
}