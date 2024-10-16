static struct dentry *isofs_export_get_parent(struct dentry *child)
{
	unsigned long parent_block = 0;
	unsigned long parent_offset = 0;
	struct inode *child_inode = child->d_inode;
	struct iso_inode_info *e_child_inode = ISOFS_I(child_inode);
	struct iso_directory_record *de = NULL;
	struct buffer_head * bh = NULL;
	struct dentry *rv = NULL;

	/* "child" must always be a directory. */
	if (!S_ISDIR(child_inode->i_mode)) {
		printk(KERN_ERR "isofs: isofs_export_get_parent(): "
		       "child is not a directory!\n");
		rv = ERR_PTR(-EACCES);
		goto out;
	}

	/* It is an invariant that the directory offset is zero.  If
	 * it is not zero, it means the directory failed to be
	 * normalized for some reason. */
	if (e_child_inode->i_iget5_offset != 0) {
		printk(KERN_ERR "isofs: isofs_export_get_parent(): "
		       "child directory not normalized!\n");
		rv = ERR_PTR(-EACCES);
		goto out;
	}

	/* The child inode has been normalized such that its
	 * i_iget5_block value points to the "." entry.  Fortunately,
	 * the ".." entry is located in the same block. */
	parent_block = e_child_inode->i_iget5_block;

	/* Get the block in question. */
	bh = sb_bread(child_inode->i_sb, parent_block);
	if (bh == NULL) {
		rv = ERR_PTR(-EACCES);
		goto out;
	}

	/* This is the "." entry. */
	de = (struct iso_directory_record*)bh->b_data;

	/* The ".." entry is always the second entry. */
	parent_offset = (unsigned long)isonum_711(de->length);
	de = (struct iso_directory_record*)(bh->b_data + parent_offset);

	/* Verify it is in fact the ".." entry. */
	if ((isonum_711(de->name_len) != 1) || (de->name[0] != 1)) {
		printk(KERN_ERR "isofs: Unable to find the \"..\" "
		       "directory for NFS.\n");
		rv = ERR_PTR(-EACCES);
		goto out;
	}

	/* Normalize */
	isofs_normalize_block_and_offset(de, &parent_block, &parent_offset);

	rv = d_obtain_alias(isofs_iget(child_inode->i_sb, parent_block,
				     parent_offset));
 out:
	if (bh)
		brelse(bh);
	return rv;
}