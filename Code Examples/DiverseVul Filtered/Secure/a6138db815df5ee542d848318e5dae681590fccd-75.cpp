bool fs_fully_visible(struct file_system_type *type)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;
	bool visible = false;

	if (unlikely(!ns))
		return false;

	down_read(&namespace_sem);
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		struct mount *child;
		if (mnt->mnt.mnt_sb->s_type != type)
			continue;

		/* This mount is not fully visible if there are any child mounts
		 * that cover anything except for empty directories.
		 */
		list_for_each_entry(child, &mnt->mnt_mounts, mnt_child) {
			struct inode *inode = child->mnt_mountpoint->d_inode;
			if (!S_ISDIR(inode->i_mode))
				goto next;
			if (inode->i_nlink > 2)
				goto next;
		}
		visible = true;
		goto found;
	next:	;
	}
found:
	up_read(&namespace_sem);
	return visible;
}