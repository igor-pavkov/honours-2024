static int truncate_inline_extent(struct inode *inode,
				  struct btrfs_path *path,
				  struct btrfs_key *found_key,
				  const u64 item_end,
				  const u64 new_size)
{
	struct extent_buffer *leaf = path->nodes[0];
	int slot = path->slots[0];
	struct btrfs_file_extent_item *fi;
	u32 size = (u32)(new_size - found_key->offset);
	struct btrfs_root *root = BTRFS_I(inode)->root;

	fi = btrfs_item_ptr(leaf, slot, struct btrfs_file_extent_item);

	if (btrfs_file_extent_compression(leaf, fi) != BTRFS_COMPRESS_NONE) {
		loff_t offset = new_size;
		loff_t page_end = ALIGN(offset, PAGE_CACHE_SIZE);

		/*
		 * Zero out the remaining of the last page of our inline extent,
		 * instead of directly truncating our inline extent here - that
		 * would be much more complex (decompressing all the data, then
		 * compressing the truncated data, which might be bigger than
		 * the size of the inline extent, resize the extent, etc).
		 * We release the path because to get the page we might need to
		 * read the extent item from disk (data not in the page cache).
		 */
		btrfs_release_path(path);
		return btrfs_truncate_page(inode, offset, page_end - offset, 0);
	}

	btrfs_set_file_extent_ram_bytes(leaf, fi, size);
	size = btrfs_file_extent_calc_inline_size(size);
	btrfs_truncate_item(root, path, size, 1);

	if (test_bit(BTRFS_ROOT_REF_COWS, &root->state))
		inode_sub_bytes(inode, item_end + 1 - new_size);

	return 0;
}