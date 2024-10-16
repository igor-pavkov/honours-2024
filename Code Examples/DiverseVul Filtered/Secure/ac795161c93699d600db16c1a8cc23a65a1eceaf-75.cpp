static int xdr_decode(struct nfs_readdir_descriptor *desc,
		      struct nfs_entry *entry, struct xdr_stream *xdr)
{
	struct inode *inode = file_inode(desc->file);
	int error;

	error = NFS_PROTO(inode)->decode_dirent(xdr, entry, desc->plus);
	if (error)
		return error;
	entry->fattr->time_start = desc->timestamp;
	entry->fattr->gencount = desc->gencount;
	return 0;
}