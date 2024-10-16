static int udf_symlink(struct inode *dir, struct dentry *dentry,
		       const char *symname)
{
	struct inode *inode;
	struct pathComponent *pc;
	const char *compstart;
	struct udf_fileident_bh fibh;
	struct extent_position epos = {};
	int eoffset, elen = 0;
	struct fileIdentDesc *fi;
	struct fileIdentDesc cfi;
	uint8_t *ea;
	int err;
	int block;
	unsigned char *name = NULL;
	int namelen;
	struct udf_inode_info *iinfo;
	struct super_block *sb = dir->i_sb;

	inode = udf_new_inode(dir, S_IFLNK | S_IRWXUGO, &err);
	if (!inode)
		goto out;

	iinfo = UDF_I(inode);
	down_write(&iinfo->i_data_sem);
	name = kmalloc(UDF_NAME_LEN, GFP_NOFS);
	if (!name) {
		err = -ENOMEM;
		goto out_no_entry;
	}

	inode->i_data.a_ops = &udf_symlink_aops;
	inode->i_op = &udf_symlink_inode_operations;

	if (iinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB) {
		struct kernel_lb_addr eloc;
		uint32_t bsize;

		block = udf_new_block(sb, inode,
				iinfo->i_location.partitionReferenceNum,
				iinfo->i_location.logicalBlockNum, &err);
		if (!block)
			goto out_no_entry;
		epos.block = iinfo->i_location;
		epos.offset = udf_file_entry_alloc_offset(inode);
		epos.bh = NULL;
		eloc.logicalBlockNum = block;
		eloc.partitionReferenceNum =
				iinfo->i_location.partitionReferenceNum;
		bsize = sb->s_blocksize;
		iinfo->i_lenExtents = bsize;
		udf_add_aext(inode, &epos, &eloc, bsize, 0);
		brelse(epos.bh);

		block = udf_get_pblock(sb, block,
				iinfo->i_location.partitionReferenceNum,
				0);
		epos.bh = udf_tgetblk(sb, block);
		lock_buffer(epos.bh);
		memset(epos.bh->b_data, 0x00, bsize);
		set_buffer_uptodate(epos.bh);
		unlock_buffer(epos.bh);
		mark_buffer_dirty_inode(epos.bh, inode);
		ea = epos.bh->b_data + udf_ext0_offset(inode);
	} else
		ea = iinfo->i_ext.i_data + iinfo->i_lenEAttr;

	eoffset = sb->s_blocksize - udf_ext0_offset(inode);
	pc = (struct pathComponent *)ea;

	if (*symname == '/') {
		do {
			symname++;
		} while (*symname == '/');

		pc->componentType = 1;
		pc->lengthComponentIdent = 0;
		pc->componentFileVersionNum = 0;
		elen += sizeof(struct pathComponent);
	}

	err = -ENAMETOOLONG;

	while (*symname) {
		if (elen + sizeof(struct pathComponent) > eoffset)
			goto out_no_entry;

		pc = (struct pathComponent *)(ea + elen);

		compstart = symname;

		do {
			symname++;
		} while (*symname && *symname != '/');

		pc->componentType = 5;
		pc->lengthComponentIdent = 0;
		pc->componentFileVersionNum = 0;
		if (compstart[0] == '.') {
			if ((symname - compstart) == 1)
				pc->componentType = 4;
			else if ((symname - compstart) == 2 &&
					compstart[1] == '.')
				pc->componentType = 3;
		}

		if (pc->componentType == 5) {
			namelen = udf_put_filename(sb, compstart, name,
						   symname - compstart);
			if (!namelen)
				goto out_no_entry;

			if (elen + sizeof(struct pathComponent) + namelen >
					eoffset)
				goto out_no_entry;
			else
				pc->lengthComponentIdent = namelen;

			memcpy(pc->componentIdent, name, namelen);
		}

		elen += sizeof(struct pathComponent) + pc->lengthComponentIdent;

		if (*symname) {
			do {
				symname++;
			} while (*symname == '/');
		}
	}

	brelse(epos.bh);
	inode->i_size = elen;
	if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		iinfo->i_lenAlloc = inode->i_size;
	else
		udf_truncate_tail_extent(inode);
	mark_inode_dirty(inode);

	fi = udf_add_entry(dir, dentry, &fibh, &cfi, &err);
	if (!fi)
		goto out_no_entry;
	cfi.icb.extLength = cpu_to_le32(sb->s_blocksize);
	cfi.icb.extLocation = cpu_to_lelb(iinfo->i_location);
	if (UDF_SB(inode->i_sb)->s_lvid_bh) {
		*(__le32 *)((struct allocDescImpUse *)cfi.icb.impUse)->impUse =
			cpu_to_le32(lvid_get_unique_id(sb));
	}
	udf_write_fi(dir, &cfi, fi, &fibh, NULL, NULL);
	if (UDF_I(dir)->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		mark_inode_dirty(dir);
	up_write(&iinfo->i_data_sem);
	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);
	d_instantiate(dentry, inode);
	err = 0;

out:
	kfree(name);
	return err;

out_no_entry:
	up_write(&iinfo->i_data_sem);
	inode_dec_link_count(inode);
	iput(inode);
	goto out;
}