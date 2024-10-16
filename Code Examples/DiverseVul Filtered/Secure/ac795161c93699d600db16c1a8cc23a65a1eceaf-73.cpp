struct dentry *nfs_lookup(struct inode *dir, struct dentry * dentry, unsigned int flags)
{
	struct dentry *res;
	struct inode *inode = NULL;
	struct nfs_fh *fhandle = NULL;
	struct nfs_fattr *fattr = NULL;
	unsigned long dir_verifier;
	int error;

	dfprintk(VFS, "NFS: lookup(%pd2)\n", dentry);
	nfs_inc_stats(dir, NFSIOS_VFSLOOKUP);

	if (unlikely(dentry->d_name.len > NFS_SERVER(dir)->namelen))
		return ERR_PTR(-ENAMETOOLONG);

	/*
	 * If we're doing an exclusive create, optimize away the lookup
	 * but don't hash the dentry.
	 */
	if (nfs_is_exclusive_create(dir, flags) || flags & LOOKUP_RENAME_TARGET)
		return NULL;

	res = ERR_PTR(-ENOMEM);
	fhandle = nfs_alloc_fhandle();
	fattr = nfs_alloc_fattr_with_label(NFS_SERVER(dir));
	if (fhandle == NULL || fattr == NULL)
		goto out;

	dir_verifier = nfs_save_change_attribute(dir);
	trace_nfs_lookup_enter(dir, dentry, flags);
	error = NFS_PROTO(dir)->lookup(dir, dentry, fhandle, fattr);
	if (error == -ENOENT) {
		if (nfs_server_capable(dir, NFS_CAP_CASE_INSENSITIVE))
			dir_verifier = inode_peek_iversion_raw(dir);
		goto no_entry;
	}
	if (error < 0) {
		res = ERR_PTR(error);
		goto out;
	}
	inode = nfs_fhget(dentry->d_sb, fhandle, fattr);
	res = ERR_CAST(inode);
	if (IS_ERR(res))
		goto out;

	/* Notify readdir to use READDIRPLUS */
	nfs_force_use_readdirplus(dir);

no_entry:
	res = d_splice_alias(inode, dentry);
	if (res != NULL) {
		if (IS_ERR(res))
			goto out;
		dentry = res;
	}
	nfs_set_verifier(dentry, dir_verifier);
out:
	trace_nfs_lookup_exit(dir, dentry, flags, PTR_ERR_OR_ZERO(res));
	nfs_free_fattr(fattr);
	nfs_free_fhandle(fhandle);
	return res;
}