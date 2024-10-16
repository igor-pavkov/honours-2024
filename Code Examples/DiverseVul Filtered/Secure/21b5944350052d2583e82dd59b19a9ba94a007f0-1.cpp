static void netns_put(struct ns_common *ns)
{
	put_net(to_net_ns(ns));
}