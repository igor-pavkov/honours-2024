int jbd2_journal_inode_add_write(handle_t *handle, struct jbd2_inode *jinode)
{
	return jbd2_journal_file_inode(handle, jinode,
				       JI_WRITE_DATA | JI_WAIT_DATA);
}