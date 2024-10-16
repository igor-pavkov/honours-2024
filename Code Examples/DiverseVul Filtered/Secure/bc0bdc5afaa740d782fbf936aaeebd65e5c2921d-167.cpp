int rdma_resolve_addr(struct rdma_cm_id *id, struct sockaddr *src_addr,
		      const struct sockaddr *dst_addr, unsigned long timeout_ms)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);
	int ret;

	ret = resolve_prepare_src(id_priv, src_addr, dst_addr);
	if (ret)
		return ret;

	if (cma_any_addr(dst_addr)) {
		ret = cma_resolve_loopback(id_priv);
	} else {
		if (dst_addr->sa_family == AF_IB) {
			ret = cma_resolve_ib_addr(id_priv);
		} else {
			ret = rdma_resolve_ip(cma_src_addr(id_priv), dst_addr,
					      &id->route.addr.dev_addr,
					      timeout_ms, addr_handler,
					      false, id_priv);
		}
	}
	if (ret)
		goto err;

	return 0;
err:
	cma_comp_exch(id_priv, RDMA_CM_ADDR_QUERY, RDMA_CM_ADDR_BOUND);
	return ret;
}