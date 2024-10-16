static void cma_save_ip4_info(struct sockaddr_in *src_addr,
			      struct sockaddr_in *dst_addr,
			      struct cma_hdr *hdr,
			      __be16 local_port)
{
	if (src_addr) {
		*src_addr = (struct sockaddr_in) {
			.sin_family = AF_INET,
			.sin_addr.s_addr = hdr->dst_addr.ip4.addr,
			.sin_port = local_port,
		};
	}

	if (dst_addr) {
		*dst_addr = (struct sockaddr_in) {
			.sin_family = AF_INET,
			.sin_addr.s_addr = hdr->src_addr.ip4.addr,
			.sin_port = hdr->port,
		};
	}
}