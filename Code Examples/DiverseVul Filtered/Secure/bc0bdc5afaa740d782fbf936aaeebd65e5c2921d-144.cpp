static void cma_save_ip6_info(struct sockaddr_in6 *src_addr,
			      struct sockaddr_in6 *dst_addr,
			      struct cma_hdr *hdr,
			      __be16 local_port)
{
	if (src_addr) {
		*src_addr = (struct sockaddr_in6) {
			.sin6_family = AF_INET6,
			.sin6_addr = hdr->dst_addr.ip6,
			.sin6_port = local_port,
		};
	}

	if (dst_addr) {
		*dst_addr = (struct sockaddr_in6) {
			.sin6_family = AF_INET6,
			.sin6_addr = hdr->src_addr.ip6,
			.sin6_port = hdr->port,
		};
	}
}