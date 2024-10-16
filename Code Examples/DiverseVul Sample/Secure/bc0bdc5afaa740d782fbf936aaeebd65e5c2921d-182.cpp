static void cma_save_ib_info(struct sockaddr *src_addr,
			     struct sockaddr *dst_addr,
			     const struct rdma_cm_id *listen_id,
			     const struct sa_path_rec *path)
{
	struct sockaddr_ib *listen_ib, *ib;

	listen_ib = (struct sockaddr_ib *) &listen_id->route.addr.src_addr;
	if (src_addr) {
		ib = (struct sockaddr_ib *)src_addr;
		ib->sib_family = AF_IB;
		if (path) {
			ib->sib_pkey = path->pkey;
			ib->sib_flowinfo = path->flow_label;
			memcpy(&ib->sib_addr, &path->sgid, 16);
			ib->sib_sid = path->service_id;
			ib->sib_scope_id = 0;
		} else {
			ib->sib_pkey = listen_ib->sib_pkey;
			ib->sib_flowinfo = listen_ib->sib_flowinfo;
			ib->sib_addr = listen_ib->sib_addr;
			ib->sib_sid = listen_ib->sib_sid;
			ib->sib_scope_id = listen_ib->sib_scope_id;
		}
		ib->sib_sid_mask = cpu_to_be64(0xffffffffffffffffULL);
	}
	if (dst_addr) {
		ib = (struct sockaddr_ib *)dst_addr;
		ib->sib_family = AF_IB;
		if (path) {
			ib->sib_pkey = path->pkey;
			ib->sib_flowinfo = path->flow_label;
			memcpy(&ib->sib_addr, &path->dgid, 16);
		}
	}
}