static struct file_id vfswrap_file_id_create(struct vfs_handle_struct *handle,
					     const SMB_STRUCT_STAT *sbuf)
{
	struct file_id key;

	/* the ZERO_STRUCT ensures padding doesn't break using the key as a
	 * blob */
	ZERO_STRUCT(key);

	key.devid = sbuf->st_ex_dev;
	key.inode = sbuf->st_ex_ino;
	/* key.extid is unused by default. */

	return key;
}