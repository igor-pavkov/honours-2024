static __be32 cma_get_roce_udp_flow_label(struct rdma_id_private *id_priv)
{
	struct sockaddr_in6 *addr6;
	u16 dport, sport;
	u32 hash, fl;

	addr6 = (struct sockaddr_in6 *)cma_src_addr(id_priv);
	fl = be32_to_cpu(addr6->sin6_flowinfo) & IB_GRH_FLOWLABEL_MASK;
	if ((cma_family(id_priv) != AF_INET6) || !fl) {
		dport = be16_to_cpu(cma_port(cma_dst_addr(id_priv)));
		sport = be16_to_cpu(cma_port(cma_src_addr(id_priv)));
		hash = (u32)sport * 31 + dport;
		fl = hash & IB_GRH_FLOWLABEL_MASK;
	}

	return cpu_to_be32(fl);
}