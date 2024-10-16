int jbd2_journal_inode_add_wait(handle_t *handle, struct jbd2_inode *jinode)
{
	return jbd2_journal_file_inode(handle, jinode, JI_WAIT_DATA);
}