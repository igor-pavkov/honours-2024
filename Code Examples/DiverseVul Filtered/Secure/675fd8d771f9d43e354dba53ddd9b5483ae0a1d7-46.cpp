static void convert_sbuf(vfs_handle_struct *handle, const char *fname,
			 SMB_STRUCT_STAT *sbuf)
{
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return);

	if (config->fixinodes) {
		/* some snapshot systems, like GPFS, return the name
		   device:inode for the snapshot files as the current
		   files. That breaks the 'restore' button in the shadow copy
		   GUI, as the client gets a sharing violation.

		   This is a crude way of allowing both files to be
		   open at once. It has a slight chance of inode
		   number collision, but I can't see a better approach
		   without significant VFS changes
		*/
		TDB_DATA key = { .dptr = discard_const_p(uint8_t, fname),
				 .dsize = strlen(fname) };
		uint32_t shash;

		shash = tdb_jenkins_hash(&key) & 0xFF000000;
		if (shash == 0) {
			shash = 1;
		}
		sbuf->st_ex_ino ^= shash;
	}
}