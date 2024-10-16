static void attach_mnt(struct vfsmount *mnt, struct nameidata *nd)
{
	mnt_set_mountpoint(nd->mnt, nd->dentry, mnt);
	list_add_tail(&mnt->mnt_hash, mount_hashtable +
			hash(nd->mnt, nd->dentry));
	list_add_tail(&mnt->mnt_child, &nd->mnt->mnt_mounts);
}