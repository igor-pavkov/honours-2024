struct mnt_namespace *to_mnt_ns(struct ns_common *ns)
{
	return container_of(ns, struct mnt_namespace, ns);
}