static struct fs_struct *__copy_fs_struct(struct fs_struct *old)
{
	struct fs_struct *fs = kmem_cache_alloc(fs_cachep, GFP_KERNEL);
	/* We don't need to lock fs - think why ;-) */
	if (fs) {
		atomic_set(&fs->count, 1);
		rwlock_init(&fs->lock);
		fs->umask = old->umask;
		read_lock(&old->lock);
		fs->root = old->root;
		path_get(&old->root);
		fs->pwd = old->pwd;
		path_get(&old->pwd);
		read_unlock(&old->lock);
	}
	return fs;
}