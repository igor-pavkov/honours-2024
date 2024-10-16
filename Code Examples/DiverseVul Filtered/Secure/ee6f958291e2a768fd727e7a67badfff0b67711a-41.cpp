void __put_mnt_ns(struct mnt_namespace *ns)
{
	struct vfsmount *root = ns->root;
	LIST_HEAD(umount_list);
	ns->root = NULL;
	spin_unlock(&vfsmount_lock);
	down_write(&namespace_sem);
	spin_lock(&vfsmount_lock);
	umount_tree(root, 0, &umount_list);
	spin_unlock(&vfsmount_lock);
	up_write(&namespace_sem);
	release_mounts(&umount_list);
	kfree(ns);
}