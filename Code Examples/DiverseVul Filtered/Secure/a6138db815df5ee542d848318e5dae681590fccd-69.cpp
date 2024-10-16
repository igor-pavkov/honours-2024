void put_mnt_ns(struct mnt_namespace *ns)
{
	if (!atomic_dec_and_test(&ns->count))
		return;
	drop_collected_mounts(&ns->root->mnt);
	free_mnt_ns(ns);
}