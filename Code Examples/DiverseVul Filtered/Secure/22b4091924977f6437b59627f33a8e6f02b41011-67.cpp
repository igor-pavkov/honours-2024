static unsigned int count_acl_entries(connection_struct *conn, SMB_ACL_T posix_acl)
{
	unsigned int ace_count = 0;
	int entry_id = SMB_ACL_FIRST_ENTRY;
	SMB_ACL_ENTRY_T entry;

	while ( posix_acl && (sys_acl_get_entry(posix_acl, entry_id, &entry) == 1)) {
		/* get_next... */
		if (entry_id == SMB_ACL_FIRST_ENTRY) {
			entry_id = SMB_ACL_NEXT_ENTRY;
		}
		ace_count++;
	}
	return ace_count;
}