static int nfs_fsync_dir(struct file *filp, loff_t start, loff_t end,
			 int datasync)
{
	dfprintk(FILE, "NFS: fsync dir(%pD2) datasync %d\n", filp, datasync);

	nfs_inc_stats(file_inode(filp), NFSIOS_VFSFSYNC);
	return 0;
}