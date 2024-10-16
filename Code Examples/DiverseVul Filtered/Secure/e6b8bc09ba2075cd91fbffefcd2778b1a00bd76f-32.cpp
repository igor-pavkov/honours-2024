static struct buffer_head * ext4_dx_find_entry(struct inode *dir, const struct qstr *d_name,
		       struct ext4_dir_entry_2 **res_dir, int *err)
{
	struct super_block * sb;
	struct dx_hash_info	hinfo;
	u32 hash;
	struct dx_frame frames[2], *frame;
	struct ext4_dir_entry_2 *de, *top;
	struct buffer_head *bh;
	ext4_lblk_t block;
	int retval;
	int namelen = d_name->len;
	const u8 *name = d_name->name;

	sb = dir->i_sb;
	/* NFS may look up ".." - look at dx_root directory block */
	if (namelen > 2 || name[0] != '.'||(name[1] != '.' && name[1] != '\0')){
		if (!(frame = dx_probe(d_name, dir, &hinfo, frames, err)))
			return NULL;
	} else {
		frame = frames;
		frame->bh = NULL;			/* for dx_release() */
		frame->at = (struct dx_entry *)frames;	/* hack for zero entry*/
		dx_set_block(frame->at, 0);		/* dx_root block is 0 */
	}
	hash = hinfo.hash;
	do {
		block = dx_get_block(frame->at);
		if (!(bh = ext4_bread (NULL,dir, block, 0, err)))
			goto errout;
		de = (struct ext4_dir_entry_2 *) bh->b_data;
		top = (struct ext4_dir_entry_2 *) ((char *) de + sb->s_blocksize -
				       EXT4_DIR_REC_LEN(0));
		for (; de < top; de = ext4_next_entry(de)) {
			int off = (block << EXT4_BLOCK_SIZE_BITS(sb))
				  + ((char *) de - bh->b_data);

			if (!ext4_check_dir_entry(__func__, dir, de, bh, off)) {
				brelse(bh);
				*err = ERR_BAD_DX_DIR;
				goto errout;
			}

			if (ext4_match(namelen, name, de)) {
				*res_dir = de;
				dx_release(frames);
				return bh;
			}
		}
		brelse(bh);
		/* Check to see if we should continue to search */
		retval = ext4_htree_next_block(dir, hash, frame,
					       frames, NULL);
		if (retval < 0) {
			ext4_warning(sb, __func__,
			     "error reading index page in directory #%lu",
			     dir->i_ino);
			*err = retval;
			goto errout;
		}
	} while (retval == 1);

	*err = -ENOENT;
errout:
	dxtrace(printk(KERN_DEBUG "%s not found\n", name));
	dx_release (frames);
	return NULL;
}