static int cma_save_ip_info(struct sockaddr *src_addr,
			    struct sockaddr *dst_addr,
			    const struct ib_cm_event *ib_event,
			    __be64 service_id)
{
	struct cma_hdr *hdr;
	__be16 port;

	hdr = ib_event->private_data;
	if (hdr->cma_version != CMA_VERSION)
		return -EINVAL;

	port = htons(cma_port_from_service_id(service_id));

	switch (cma_get_ip_ver(hdr)) {
	case 4:
		cma_save_ip4_info((struct sockaddr_in *)src_addr,
				  (struct sockaddr_in *)dst_addr, hdr, port);
		break;
	case 6:
		cma_save_ip6_info((struct sockaddr_in6 *)src_addr,
				  (struct sockaddr_in6 *)dst_addr, hdr, port);
		break;
	default:
		return -EAFNOSUPPORT;
	}

	return 0;
}