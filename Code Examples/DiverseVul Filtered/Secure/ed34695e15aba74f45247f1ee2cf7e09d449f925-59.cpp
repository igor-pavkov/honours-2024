static void ff_layout_free_mirror(struct nfs4_ff_layout_mirror *mirror)
{
	const struct cred	*cred;

	ff_layout_remove_mirror(mirror);
	kfree(mirror->fh_versions);
	cred = rcu_access_pointer(mirror->ro_cred);
	put_cred(cred);
	cred = rcu_access_pointer(mirror->rw_cred);
	put_cred(cred);
	nfs4_ff_layout_put_deviceid(mirror->mirror_ds);
	kfree(mirror);
}