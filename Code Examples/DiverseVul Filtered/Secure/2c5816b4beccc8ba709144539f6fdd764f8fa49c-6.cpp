static void cuse_fc_release(struct fuse_conn *fc)
{
	struct cuse_conn *cc = fc_to_cc(fc);
	kfree_rcu(cc, fc.rcu);
}