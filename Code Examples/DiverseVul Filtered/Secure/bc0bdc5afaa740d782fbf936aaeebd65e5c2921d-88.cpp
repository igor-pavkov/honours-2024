static int cma_save_net_info(struct sockaddr *src_addr,
			     struct sockaddr *dst_addr,
			     const struct rdma_cm_id *listen_id,
			     const struct ib_cm_event *ib_event,
			     sa_family_t sa_family, __be64 service_id)
{
	if (sa_family == AF_IB) {
		if (ib_event->event == IB_CM_REQ_RECEIVED)
			cma_save_ib_info(src_addr, dst_addr, listen_id,
					 ib_event->param.req_rcvd.primary_path);
		else if (ib_event->event == IB_CM_SIDR_REQ_RECEIVED)
			cma_save_ib_info(src_addr, dst_addr, listen_id, NULL);
		return 0;
	}

	return cma_save_ip_info(src_addr, dst_addr, ib_event, service_id);
}