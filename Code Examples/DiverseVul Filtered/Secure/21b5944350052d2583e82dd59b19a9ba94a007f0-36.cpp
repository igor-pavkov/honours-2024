static struct ucounts *inc_net_namespaces(struct user_namespace *ns)
{
	return inc_ucount(ns, current_euid(), UCOUNT_NET_NAMESPACES);
}