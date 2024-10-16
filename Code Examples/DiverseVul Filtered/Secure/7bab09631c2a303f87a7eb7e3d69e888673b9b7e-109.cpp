static void xfrm_init_pmtu(struct dst_entry *dst)
{
	do {
		struct xfrm_dst *xdst = (struct xfrm_dst *)dst;
		u32 pmtu, route_mtu_cached;

		pmtu = dst_mtu(dst->child);
		xdst->child_mtu_cached = pmtu;

		pmtu = xfrm_state_mtu(dst->xfrm, pmtu);

		route_mtu_cached = dst_mtu(xdst->route);
		xdst->route_mtu_cached = route_mtu_cached;

		if (pmtu > route_mtu_cached)
			pmtu = route_mtu_cached;

		dst_metric_set(dst, RTAX_MTU, pmtu);
	} while ((dst = dst->next));
}