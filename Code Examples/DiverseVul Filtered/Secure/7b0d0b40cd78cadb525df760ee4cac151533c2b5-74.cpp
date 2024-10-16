static int selinux_sb_clone_mnt_opts(const struct super_block *oldsb,
					struct super_block *newsb)
{
	const struct superblock_security_struct *oldsbsec = oldsb->s_security;
	struct superblock_security_struct *newsbsec = newsb->s_security;

	int set_fscontext =	(oldsbsec->flags & FSCONTEXT_MNT);
	int set_context =	(oldsbsec->flags & CONTEXT_MNT);
	int set_rootcontext =	(oldsbsec->flags & ROOTCONTEXT_MNT);

	/*
	 * if the parent was able to be mounted it clearly had no special lsm
	 * mount options.  thus we can safely deal with this superblock later
	 */
	if (!ss_initialized)
		return 0;

	/* how can we clone if the old one wasn't set up?? */
	BUG_ON(!(oldsbsec->flags & SE_SBINITIALIZED));

	/* if fs is reusing a sb, make sure that the contexts match */
	if (newsbsec->flags & SE_SBINITIALIZED)
		return selinux_cmp_sb_context(oldsb, newsb);

	mutex_lock(&newsbsec->lock);

	newsbsec->flags = oldsbsec->flags;

	newsbsec->sid = oldsbsec->sid;
	newsbsec->def_sid = oldsbsec->def_sid;
	newsbsec->behavior = oldsbsec->behavior;

	if (set_context) {
		u32 sid = oldsbsec->mntpoint_sid;

		if (!set_fscontext)
			newsbsec->sid = sid;
		if (!set_rootcontext) {
			struct inode *newinode = newsb->s_root->d_inode;
			struct inode_security_struct *newisec = newinode->i_security;
			newisec->sid = sid;
		}
		newsbsec->mntpoint_sid = sid;
	}
	if (set_rootcontext) {
		const struct inode *oldinode = oldsb->s_root->d_inode;
		const struct inode_security_struct *oldisec = oldinode->i_security;
		struct inode *newinode = newsb->s_root->d_inode;
		struct inode_security_struct *newisec = newinode->i_security;

		newisec->sid = oldisec->sid;
	}

	sb_finish_set_opts(newsb);
	mutex_unlock(&newsbsec->lock);
	return 0;
}