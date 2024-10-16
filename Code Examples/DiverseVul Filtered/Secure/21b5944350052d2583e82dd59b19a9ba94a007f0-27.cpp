static struct user_namespace *netns_owner(struct ns_common *ns)
{
	return to_net_ns(ns)->user_ns;
}