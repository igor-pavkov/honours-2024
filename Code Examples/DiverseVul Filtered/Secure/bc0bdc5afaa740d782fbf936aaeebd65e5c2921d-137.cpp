static int cma_addr_cmp(const struct sockaddr *src, const struct sockaddr *dst)
{
	if (src->sa_family != dst->sa_family)
		return -1;

	switch (src->sa_family) {
	case AF_INET:
		return ((struct sockaddr_in *)src)->sin_addr.s_addr !=
		       ((struct sockaddr_in *)dst)->sin_addr.s_addr;
	case AF_INET6: {
		struct sockaddr_in6 *src_addr6 = (struct sockaddr_in6 *)src;
		struct sockaddr_in6 *dst_addr6 = (struct sockaddr_in6 *)dst;
		bool link_local;

		if (ipv6_addr_cmp(&src_addr6->sin6_addr,
					  &dst_addr6->sin6_addr))
			return 1;
		link_local = ipv6_addr_type(&dst_addr6->sin6_addr) &
			     IPV6_ADDR_LINKLOCAL;
		/* Link local must match their scope_ids */
		return link_local ? (src_addr6->sin6_scope_id !=
				     dst_addr6->sin6_scope_id) :
				    0;
	}

	default:
		return ib_addr_cmp(&((struct sockaddr_ib *) src)->sib_addr,
				   &((struct sockaddr_ib *) dst)->sib_addr);
	}
}