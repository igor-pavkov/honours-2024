static int __btrfs_prealloc_file_range(struct inode *inode, int mode,
				       u64 start, u64 num_bytes, u64 min_size,
				       loff_t actual_len, u64 *alloc_hint,
				       struct btrfs_trans_handle *trans)
{
	struct extent_map_tree *em_tree = &BTRFS_I(inode)->extent_tree;
	struct extent_map *em;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_key ins;
	u64 cur_offset = start;
	u64 i_size;
	u64 cur_bytes;
	int ret = 0;
	bool own_trans = true;

	if (trans)
		own_trans = false;
	while (num_bytes > 0) {
		if (own_trans) {
			trans = btrfs_start_transaction(root, 3);
			if (IS_ERR(trans)) {
				ret = PTR_ERR(trans);
				break;
			}
		}

		cur_bytes = min(num_bytes, 256ULL * 1024 * 1024);
		cur_bytes = max(cur_bytes, min_size);
		ret = btrfs_reserve_extent(root, cur_bytes, min_size, 0,
					   *alloc_hint, &ins, 1, 0);
		if (ret) {
			if (own_trans)
				btrfs_end_transaction(trans, root);
			break;
		}

		ret = insert_reserved_file_extent(trans, inode,
						  cur_offset, ins.objectid,
						  ins.offset, ins.offset,
						  ins.offset, 0, 0, 0,
						  BTRFS_FILE_EXTENT_PREALLOC);
		if (ret) {
			btrfs_free_reserved_extent(root, ins.objectid,
						   ins.offset, 0);
			btrfs_abort_transaction(trans, root, ret);
			if (own_trans)
				btrfs_end_transaction(trans, root);
			break;
		}

		btrfs_drop_extent_cache(inode, cur_offset,
					cur_offset + ins.offset -1, 0);

		em = alloc_extent_map();
		if (!em) {
			set_bit(BTRFS_INODE_NEEDS_FULL_SYNC,
				&BTRFS_I(inode)->runtime_flags);
			goto next;
		}

		em->start = cur_offset;
		em->orig_start = cur_offset;
		em->len = ins.offset;
		em->block_start = ins.objectid;
		em->block_len = ins.offset;
		em->orig_block_len = ins.offset;
		em->ram_bytes = ins.offset;
		em->bdev = root->fs_info->fs_devices->latest_bdev;
		set_bit(EXTENT_FLAG_PREALLOC, &em->flags);
		em->generation = trans->transid;

		while (1) {
			write_lock(&em_tree->lock);
			ret = add_extent_mapping(em_tree, em, 1);
			write_unlock(&em_tree->lock);
			if (ret != -EEXIST)
				break;
			btrfs_drop_extent_cache(inode, cur_offset,
						cur_offset + ins.offset - 1,
						0);
		}
		free_extent_map(em);
next:
		num_bytes -= ins.offset;
		cur_offset += ins.offset;
		*alloc_hint = ins.objectid + ins.offset;

		inode_inc_iversion(inode);
		inode->i_ctime = CURRENT_TIME;
		BTRFS_I(inode)->flags |= BTRFS_INODE_PREALLOC;
		if (!(mode & FALLOC_FL_KEEP_SIZE) &&
		    (actual_len > inode->i_size) &&
		    (cur_offset > inode->i_size)) {
			if (cur_offset > actual_len)
				i_size = actual_len;
			else
				i_size = cur_offset;
			i_size_write(inode, i_size);
			btrfs_ordered_update_i_size(inode, i_size, NULL);
		}

		ret = btrfs_update_inode(trans, root, inode);

		if (ret) {
			btrfs_abort_transaction(trans, root, ret);
			if (own_trans)
				btrfs_end_transaction(trans, root);
			break;
		}

		if (own_trans)
			btrfs_end_transaction(trans, root);
	}
	return ret;
}