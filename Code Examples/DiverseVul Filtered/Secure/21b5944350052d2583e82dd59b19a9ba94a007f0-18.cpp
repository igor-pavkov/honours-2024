void net_drop_ns(void *p)
{
	struct net *ns = p;
	if (ns && refcount_dec_and_test(&ns->passive))
		net_free(ns);
}