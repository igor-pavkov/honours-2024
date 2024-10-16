int nfs_same_file(struct dentry *dentry, struct nfs_entry *entry)
{
	struct inode *inode;
	struct nfs_inode *nfsi;

	if (d_really_is_negative(dentry))
		return 0;

	inode = d_inode(dentry);
	if (is_bad_inode(inode) || NFS_STALE(inode))
		return 0;

	nfsi = NFS_I(inode);
	if (entry->fattr->fileid != nfsi->fileid)
		return 0;
	if (entry->fh->size && nfs_compare_fh(entry->fh, &nfsi->fh) != 0)
		return 0;
	return 1;
}