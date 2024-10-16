static bool cma_is_req_ipv6_ll(const struct cma_req_info *req)
{
	const struct sockaddr *daddr =
			(const struct sockaddr *)&req->listen_addr_storage;
	const struct sockaddr_in6 *daddr6 = (const struct sockaddr_in6 *)daddr;

	/* Returns true if the req is for IPv6 link local */
	return (daddr->sa_family == AF_INET6 &&
		(ipv6_addr_type(&daddr6->sin6_addr) & IPV6_ADDR_LINKLOCAL));
}