void rdma_read_gids(struct rdma_cm_id *cm_id, union ib_gid *sgid,
		    union ib_gid *dgid)
{
	struct rdma_addr *addr = &cm_id->route.addr;

	if (!cm_id->device) {
		if (sgid)
			memset(sgid, 0, sizeof(*sgid));
		if (dgid)
			memset(dgid, 0, sizeof(*dgid));
		return;
	}

	if (rdma_protocol_roce(cm_id->device, cm_id->port_num)) {
		if (sgid)
			rdma_ip2gid((struct sockaddr *)&addr->src_addr, sgid);
		if (dgid)
			rdma_ip2gid((struct sockaddr *)&addr->dst_addr, dgid);
	} else {
		if (sgid)
			rdma_addr_get_sgid(&addr->dev_addr, sgid);
		if (dgid)
			rdma_addr_get_dgid(&addr->dev_addr, dgid);
	}
}