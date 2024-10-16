void lockdep_annotate_inode_mutex_key(struct inode *inode)
{
	if (S_ISDIR(inode->i_mode)) {
		struct file_system_type *type = inode->i_sb->s_type;

		/* Set new key only if filesystem hasn't already changed it */
		if (lockdep_match_class(&inode->i_rwsem, &type->i_mutex_key)) {
			/*
			 * ensure nobody is actually holding i_mutex
			 */
			// mutex_destroy(&inode->i_mutex);
			init_rwsem(&inode->i_rwsem);
			lockdep_set_class(&inode->i_rwsem,
					  &type->i_mutex_dir_key);
		}
	}
}