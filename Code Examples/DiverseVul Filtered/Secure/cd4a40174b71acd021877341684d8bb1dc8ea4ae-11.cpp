static void mntns_put(struct ns_common *ns)
{
	put_mnt_ns(to_mnt_ns(ns));
}