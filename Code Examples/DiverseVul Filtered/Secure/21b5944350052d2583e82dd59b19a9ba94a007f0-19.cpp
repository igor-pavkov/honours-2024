static inline struct net *to_net_ns(struct ns_common *ns)
{
	return container_of(ns, struct net, ns);
}