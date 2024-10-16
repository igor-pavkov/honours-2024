static int do_loopback(struct nameidata *nd, char *old_name, int recurse)
{
	struct nameidata old_nd;
	struct vfsmount *mnt = NULL;
	int err = mount_is_safe(nd);
	if (err)
		return err;
	if (!old_name || !*old_name)
		return -EINVAL;
	err = path_lookup(old_name, LOOKUP_FOLLOW, &old_nd);
	if (err)
		return err;

	down_write(&namespace_sem);
	err = -EINVAL;
	if (IS_MNT_UNBINDABLE(old_nd.mnt))
 		goto out;

	if (!check_mnt(nd->mnt) || !check_mnt(old_nd.mnt))
		goto out;

	err = -ENOMEM;
	if (recurse)
		mnt = copy_tree(old_nd.mnt, old_nd.dentry, 0);
	else
		mnt = clone_mnt(old_nd.mnt, old_nd.dentry, 0);

	if (!mnt)
		goto out;

	err = graft_tree(mnt, nd);
	if (err) {
		LIST_HEAD(umount_list);
		spin_lock(&vfsmount_lock);
		umount_tree(mnt, 0, &umount_list);
		spin_unlock(&vfsmount_lock);
		release_mounts(&umount_list);
	}

out:
	up_write(&namespace_sem);
	path_release(&old_nd);
	return err;
}