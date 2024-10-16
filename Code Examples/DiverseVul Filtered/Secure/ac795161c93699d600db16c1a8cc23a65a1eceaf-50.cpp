static bool nfs_verifier_is_delegated(struct dentry *dentry)
{
	return nfs_test_verifier_delegated(dentry->d_time);
}