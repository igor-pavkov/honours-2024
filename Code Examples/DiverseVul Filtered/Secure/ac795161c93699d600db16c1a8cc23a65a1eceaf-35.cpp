static void nfs_dentry_remove_handle_error(struct inode *dir,
					   struct dentry *dentry, int error)
{
	switch (error) {
	case -ENOENT:
		d_delete(dentry);
		nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
		break;
	case 0:
		nfs_d_prune_case_insensitive_aliases(d_inode(dentry));
		nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
	}
}