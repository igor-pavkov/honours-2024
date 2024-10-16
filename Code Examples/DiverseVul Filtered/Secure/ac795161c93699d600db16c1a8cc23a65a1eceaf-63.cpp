void nfs_set_verifier(struct dentry *dentry, unsigned long verf)
{

	spin_lock(&dentry->d_lock);
	nfs_set_verifier_locked(dentry, verf);
	spin_unlock(&dentry->d_lock);
}