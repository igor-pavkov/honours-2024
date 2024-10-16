static noinline int btrfs_ioctl_snap_destroy(struct file *file,
					     void __user *arg)
{
	struct dentry *parent = file->f_path.dentry;
	struct dentry *dentry;
	struct inode *dir = d_inode(parent);
	struct inode *inode;
	struct btrfs_root *root = BTRFS_I(dir)->root;
	struct btrfs_root *dest = NULL;
	struct btrfs_ioctl_vol_args *vol_args;
	struct btrfs_trans_handle *trans;
	struct btrfs_block_rsv block_rsv;
	u64 root_flags;
	u64 qgroup_reserved;
	int namelen;
	int ret;
	int err = 0;

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args))
		return PTR_ERR(vol_args);

	vol_args->name[BTRFS_PATH_NAME_MAX] = '\0';
	namelen = strlen(vol_args->name);
	if (strchr(vol_args->name, '/') ||
	    strncmp(vol_args->name, "..", namelen) == 0) {
		err = -EINVAL;
		goto out;
	}

	err = mnt_want_write_file(file);
	if (err)
		goto out;


	err = mutex_lock_killable_nested(&dir->i_mutex, I_MUTEX_PARENT);
	if (err == -EINTR)
		goto out_drop_write;
	dentry = lookup_one_len(vol_args->name, parent, namelen);
	if (IS_ERR(dentry)) {
		err = PTR_ERR(dentry);
		goto out_unlock_dir;
	}

	if (d_really_is_negative(dentry)) {
		err = -ENOENT;
		goto out_dput;
	}

	inode = d_inode(dentry);
	dest = BTRFS_I(inode)->root;
	if (!capable(CAP_SYS_ADMIN)) {
		/*
		 * Regular user.  Only allow this with a special mount
		 * option, when the user has write+exec access to the
		 * subvol root, and when rmdir(2) would have been
		 * allowed.
		 *
		 * Note that this is _not_ check that the subvol is
		 * empty or doesn't contain data that we wouldn't
		 * otherwise be able to delete.
		 *
		 * Users who want to delete empty subvols should try
		 * rmdir(2).
		 */
		err = -EPERM;
		if (!btrfs_test_opt(root, USER_SUBVOL_RM_ALLOWED))
			goto out_dput;

		/*
		 * Do not allow deletion if the parent dir is the same
		 * as the dir to be deleted.  That means the ioctl
		 * must be called on the dentry referencing the root
		 * of the subvol, not a random directory contained
		 * within it.
		 */
		err = -EINVAL;
		if (root == dest)
			goto out_dput;

		err = inode_permission(inode, MAY_WRITE | MAY_EXEC);
		if (err)
			goto out_dput;
	}

	/* check if subvolume may be deleted by a user */
	err = btrfs_may_delete(dir, dentry, 1);
	if (err)
		goto out_dput;

	if (btrfs_ino(inode) != BTRFS_FIRST_FREE_OBJECTID) {
		err = -EINVAL;
		goto out_dput;
	}

	mutex_lock(&inode->i_mutex);

	/*
	 * Don't allow to delete a subvolume with send in progress. This is
	 * inside the i_mutex so the error handling that has to drop the bit
	 * again is not run concurrently.
	 */
	spin_lock(&dest->root_item_lock);
	root_flags = btrfs_root_flags(&dest->root_item);
	if (dest->send_in_progress == 0) {
		btrfs_set_root_flags(&dest->root_item,
				root_flags | BTRFS_ROOT_SUBVOL_DEAD);
		spin_unlock(&dest->root_item_lock);
	} else {
		spin_unlock(&dest->root_item_lock);
		btrfs_warn(root->fs_info,
			"Attempt to delete subvolume %llu during send",
			dest->root_key.objectid);
		err = -EPERM;
		goto out_unlock_inode;
	}

	down_write(&root->fs_info->subvol_sem);

	err = may_destroy_subvol(dest);
	if (err)
		goto out_up_write;

	btrfs_init_block_rsv(&block_rsv, BTRFS_BLOCK_RSV_TEMP);
	/*
	 * One for dir inode, two for dir entries, two for root
	 * ref/backref.
	 */
	err = btrfs_subvolume_reserve_metadata(root, &block_rsv,
					       5, &qgroup_reserved, true);
	if (err)
		goto out_up_write;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		err = PTR_ERR(trans);
		goto out_release;
	}
	trans->block_rsv = &block_rsv;
	trans->bytes_reserved = block_rsv.size;

	ret = btrfs_unlink_subvol(trans, root, dir,
				dest->root_key.objectid,
				dentry->d_name.name,
				dentry->d_name.len);
	if (ret) {
		err = ret;
		btrfs_abort_transaction(trans, root, ret);
		goto out_end_trans;
	}

	btrfs_record_root_in_trans(trans, dest);

	memset(&dest->root_item.drop_progress, 0,
		sizeof(dest->root_item.drop_progress));
	dest->root_item.drop_level = 0;
	btrfs_set_root_refs(&dest->root_item, 0);

	if (!test_and_set_bit(BTRFS_ROOT_ORPHAN_ITEM_INSERTED, &dest->state)) {
		ret = btrfs_insert_orphan_item(trans,
					root->fs_info->tree_root,
					dest->root_key.objectid);
		if (ret) {
			btrfs_abort_transaction(trans, root, ret);
			err = ret;
			goto out_end_trans;
		}
	}

	ret = btrfs_uuid_tree_rem(trans, root->fs_info->uuid_root,
				  dest->root_item.uuid, BTRFS_UUID_KEY_SUBVOL,
				  dest->root_key.objectid);
	if (ret && ret != -ENOENT) {
		btrfs_abort_transaction(trans, root, ret);
		err = ret;
		goto out_end_trans;
	}
	if (!btrfs_is_empty_uuid(dest->root_item.received_uuid)) {
		ret = btrfs_uuid_tree_rem(trans, root->fs_info->uuid_root,
					  dest->root_item.received_uuid,
					  BTRFS_UUID_KEY_RECEIVED_SUBVOL,
					  dest->root_key.objectid);
		if (ret && ret != -ENOENT) {
			btrfs_abort_transaction(trans, root, ret);
			err = ret;
			goto out_end_trans;
		}
	}

out_end_trans:
	trans->block_rsv = NULL;
	trans->bytes_reserved = 0;
	ret = btrfs_end_transaction(trans, root);
	if (ret && !err)
		err = ret;
	inode->i_flags |= S_DEAD;
out_release:
	btrfs_subvolume_release_metadata(root, &block_rsv, qgroup_reserved);
out_up_write:
	up_write(&root->fs_info->subvol_sem);
	if (err) {
		spin_lock(&dest->root_item_lock);
		root_flags = btrfs_root_flags(&dest->root_item);
		btrfs_set_root_flags(&dest->root_item,
				root_flags & ~BTRFS_ROOT_SUBVOL_DEAD);
		spin_unlock(&dest->root_item_lock);
	}
out_unlock_inode:
	mutex_unlock(&inode->i_mutex);
	if (!err) {
		d_invalidate(dentry);
		btrfs_invalidate_inodes(dest);
		d_delete(dentry);
		ASSERT(dest->send_in_progress == 0);

		/* the last ref */
		if (dest->ino_cache_inode) {
			iput(dest->ino_cache_inode);
			dest->ino_cache_inode = NULL;
		}
	}
out_dput:
	dput(dentry);
out_unlock_dir:
	mutex_unlock(&dir->i_mutex);
out_drop_write:
	mnt_drop_write_file(file);
out:
	kfree(vol_args);
	return err;
}