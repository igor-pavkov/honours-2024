static void __init init_mount_tree(void)
{
	struct vfsmount *mnt;
	struct mnt_namespace *ns;

	mnt = do_kern_mount("rootfs", 0, "rootfs", NULL);
	if (IS_ERR(mnt))
		panic("Can't create rootfs");
	ns = kmalloc(sizeof(*ns), GFP_KERNEL);
	if (!ns)
		panic("Can't allocate initial namespace");
	atomic_set(&ns->count, 1);
	INIT_LIST_HEAD(&ns->list);
	init_waitqueue_head(&ns->poll);
	ns->event = 0;
	list_add(&mnt->mnt_list, &ns->list);
	ns->root = mnt;
	mnt->mnt_ns = ns;

	init_task.nsproxy->mnt_ns = ns;
	get_mnt_ns(ns);

	set_fs_pwd(current->fs, ns->root, ns->root->mnt_root);
	set_fs_root(current->fs, ns->root, ns->root->mnt_root);
}