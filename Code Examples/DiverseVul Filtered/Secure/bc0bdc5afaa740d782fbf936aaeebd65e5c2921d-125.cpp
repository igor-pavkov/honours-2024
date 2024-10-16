static bool cma_match_private_data(struct rdma_id_private *id_priv,
				   const struct cma_hdr *hdr)
{
	struct sockaddr *addr = cma_src_addr(id_priv);
	__be32 ip4_addr;
	struct in6_addr ip6_addr;

	if (cma_any_addr(addr) && !id_priv->afonly)
		return true;

	switch (addr->sa_family) {
	case AF_INET:
		ip4_addr = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
		if (cma_get_ip_ver(hdr) != 4)
			return false;
		if (!cma_any_addr(addr) &&
		    hdr->dst_addr.ip4.addr != ip4_addr)
			return false;
		break;
	case AF_INET6:
		ip6_addr = ((struct sockaddr_in6 *)addr)->sin6_addr;
		if (cma_get_ip_ver(hdr) != 6)
			return false;
		if (!cma_any_addr(addr) &&
		    memcmp(&hdr->dst_addr.ip6, &ip6_addr, sizeof(ip6_addr)))
			return false;
		break;
	case AF_IB:
		return true;
	default:
		return false;
	}

	return true;
}