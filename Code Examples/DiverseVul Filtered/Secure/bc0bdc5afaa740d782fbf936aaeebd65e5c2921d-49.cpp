struct rdma_cm_id *rdma_create_user_id(rdma_cm_event_handler event_handler,
				       void *context,
				       enum rdma_ucm_port_space ps,
				       enum ib_qp_type qp_type)
{
	struct rdma_id_private *ret;

	ret = __rdma_create_id(current->nsproxy->net_ns, event_handler, context,
			       ps, qp_type, NULL);
	if (IS_ERR(ret))
		return ERR_CAST(ret);

	rdma_restrack_set_name(&ret->res, NULL);
	return &ret->id;
}