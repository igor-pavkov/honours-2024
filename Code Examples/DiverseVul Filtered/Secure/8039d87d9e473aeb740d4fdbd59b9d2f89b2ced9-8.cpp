static int create_snapshot(struct btrfs_root *root, struct inode *dir,
			   struct dentry *dentry, char *name, int namelen,
			   u64 *async_transid, bool readonly,
			   struct btrfs_qgroup_inherit *inherit)
{
	struct inode *inode;
	struct btrfs_pending_snapshot *pending_snapshot;
	struct btrfs_trans_handle *trans;
	int ret;

	if (!test_bit(BTRFS_ROOT_REF_COWS, &root->state))
		return -EINVAL;

	atomic_inc(&root->will_be_snapshoted);
	smp_mb__after_atomic();
	btrfs_wait_for_no_snapshoting_writes(root);

	ret = btrfs_start_delalloc_inodes(root, 0);
	if (ret)
		goto out;

	btrfs_wait_ordered_extents(root, -1);

	pending_snapshot = kzalloc(sizeof(*pending_snapshot), GFP_NOFS);
	if (!pending_snapshot) {
		ret = -ENOMEM;
		goto out;
	}

	btrfs_init_block_rsv(&pending_snapshot->block_rsv,
			     BTRFS_BLOCK_RSV_TEMP);
	/*
	 * 1 - parent dir inode
	 * 2 - dir entries
	 * 1 - root item
	 * 2 - root ref/backref
	 * 1 - root of snapshot
	 * 1 - UUID item
	 */
	ret = btrfs_subvolume_reserve_metadata(BTRFS_I(dir)->root,
					&pending_snapshot->block_rsv, 8,
					&pending_snapshot->qgroup_reserved,
					false);
	if (ret)
		goto free;

	pending_snapshot->dentry = dentry;
	pending_snapshot->root = root;
	pending_snapshot->readonly = readonly;
	pending_snapshot->dir = dir;
	pending_snapshot->inherit = inherit;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto fail;
	}

	spin_lock(&root->fs_info->trans_lock);
	list_add(&pending_snapshot->list,
		 &trans->transaction->pending_snapshots);
	spin_unlock(&root->fs_info->trans_lock);
	if (async_transid) {
		*async_transid = trans->transid;
		ret = btrfs_commit_transaction_async(trans,
				     root->fs_info->extent_root, 1);
		if (ret)
			ret = btrfs_commit_transaction(trans, root);
	} else {
		ret = btrfs_commit_transaction(trans,
					       root->fs_info->extent_root);
	}
	if (ret)
		goto fail;

	ret = pending_snapshot->error;
	if (ret)
		goto fail;

	ret = btrfs_orphan_cleanup(pending_snapshot->snap);
	if (ret)
		goto fail;

	inode = btrfs_lookup_dentry(d_inode(dentry->d_parent), dentry);
	if (IS_ERR(inode)) {
		ret = PTR_ERR(inode);
		goto fail;
	}

	d_instantiate(dentry, inode);
	ret = 0;
fail:
	btrfs_subvolume_release_metadata(BTRFS_I(dir)->root,
					 &pending_snapshot->block_rsv,
					 pending_snapshot->qgroup_reserved);
free:
	kfree(pending_snapshot);
out:
	if (atomic_dec_and_test(&root->will_be_snapshoted))
		wake_up_atomic_t(&root->will_be_snapshoted);
	return ret;
}