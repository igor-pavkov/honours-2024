void __init mnt_init(unsigned long mempages)
{
	struct list_head *d;
	unsigned int nr_hash;
	int i;
	int err;

	init_rwsem(&namespace_sem);

	mnt_cache = kmem_cache_create("mnt_cache", sizeof(struct vfsmount),
			0, SLAB_HWCACHE_ALIGN | SLAB_PANIC, NULL, NULL);

	mount_hashtable = (struct list_head *)__get_free_page(GFP_ATOMIC);

	if (!mount_hashtable)
		panic("Failed to allocate mount hash table\n");

	/*
	 * Find the power-of-two list-heads that can fit into the allocation..
	 * We don't guarantee that "sizeof(struct list_head)" is necessarily
	 * a power-of-two.
	 */
	nr_hash = PAGE_SIZE / sizeof(struct list_head);
	hash_bits = 0;
	do {
		hash_bits++;
	} while ((nr_hash >> hash_bits) != 0);
	hash_bits--;

	/*
	 * Re-calculate the actual number of entries and the mask
	 * from the number of bits we can fit.
	 */
	nr_hash = 1UL << hash_bits;
	hash_mask = nr_hash - 1;

	printk("Mount-cache hash table entries: %d\n", nr_hash);

	/* And initialize the newly allocated array */
	d = mount_hashtable;
	i = nr_hash;
	do {
		INIT_LIST_HEAD(d);
		d++;
		i--;
	} while (i);
	err = sysfs_init();
	if (err)
		printk(KERN_WARNING "%s: sysfs_init error: %d\n",
			__FUNCTION__, err);
	err = subsystem_register(&fs_subsys);
	if (err)
		printk(KERN_WARNING "%s: subsystem_register error: %d\n",
			__FUNCTION__, err);
	init_rootfs();
	init_mount_tree();
}