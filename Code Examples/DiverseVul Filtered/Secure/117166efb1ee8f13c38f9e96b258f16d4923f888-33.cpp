static int get_max_inline_xattr_value_size(struct inode *inode,
					   struct ext4_iloc *iloc)
{
	struct ext4_xattr_ibody_header *header;
	struct ext4_xattr_entry *entry;
	struct ext4_inode *raw_inode;
	int free, min_offs;

	min_offs = EXT4_SB(inode->i_sb)->s_inode_size -
			EXT4_GOOD_OLD_INODE_SIZE -
			EXT4_I(inode)->i_extra_isize -
			sizeof(struct ext4_xattr_ibody_header);

	/*
	 * We need to subtract another sizeof(__u32) since an in-inode xattr
	 * needs an empty 4 bytes to indicate the gap between the xattr entry
	 * and the name/value pair.
	 */
	if (!ext4_test_inode_state(inode, EXT4_STATE_XATTR))
		return EXT4_XATTR_SIZE(min_offs -
			EXT4_XATTR_LEN(strlen(EXT4_XATTR_SYSTEM_DATA)) -
			EXT4_XATTR_ROUND - sizeof(__u32));

	raw_inode = ext4_raw_inode(iloc);
	header = IHDR(inode, raw_inode);
	entry = IFIRST(header);

	/* Compute min_offs. */
	for (; !IS_LAST_ENTRY(entry); entry = EXT4_XATTR_NEXT(entry)) {
		if (!entry->e_value_inum && entry->e_value_size) {
			size_t offs = le16_to_cpu(entry->e_value_offs);
			if (offs < min_offs)
				min_offs = offs;
		}
	}
	free = min_offs -
		((void *)entry - (void *)IFIRST(header)) - sizeof(__u32);

	if (EXT4_I(inode)->i_inline_off) {
		entry = (struct ext4_xattr_entry *)
			((void *)raw_inode + EXT4_I(inode)->i_inline_off);

		free += EXT4_XATTR_SIZE(le32_to_cpu(entry->e_value_size));
		goto out;
	}

	free -= EXT4_XATTR_LEN(strlen(EXT4_XATTR_SYSTEM_DATA));

	if (free > EXT4_XATTR_ROUND)
		free = EXT4_XATTR_SIZE(free - EXT4_XATTR_ROUND);
	else
		free = 0;

out:
	return free;
}