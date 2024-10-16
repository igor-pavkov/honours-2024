static bool ipv6_route_iter_active(struct ipv6_route_iter *iter)
{
	struct fib6_walker *w = &iter->w;
	return w->node && !(w->state == FWS_U && w->node == w->root);
}