static int attach_recursive_mnt(struct vfsmount *source_mnt,
			struct nameidata *nd, struct nameidata *parent_nd)
{
	LIST_HEAD(tree_list);
	struct vfsmount *dest_mnt = nd->mnt;
	struct dentry *dest_dentry = nd->dentry;
	struct vfsmount *child, *p;

	if (propagate_mnt(dest_mnt, dest_dentry, source_mnt, &tree_list))
		return -EINVAL;

	if (IS_MNT_SHARED(dest_mnt)) {
		for (p = source_mnt; p; p = next_mnt(p, source_mnt))
			set_mnt_shared(p);
	}

	spin_lock(&vfsmount_lock);
	if (parent_nd) {
		detach_mnt(source_mnt, parent_nd);
		attach_mnt(source_mnt, nd);
		touch_mnt_namespace(current->nsproxy->mnt_ns);
	} else {
		mnt_set_mountpoint(dest_mnt, dest_dentry, source_mnt);
		commit_tree(source_mnt);
	}

	list_for_each_entry_safe(child, p, &tree_list, mnt_hash) {
		list_del_init(&child->mnt_hash);
		commit_tree(child);
	}
	spin_unlock(&vfsmount_lock);
	return 0;
}