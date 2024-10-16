const char *__attribute_const__ rdma_reject_msg(struct rdma_cm_id *id,
						int reason)
{
	if (rdma_ib_or_roce(id->device, id->port_num))
		return ibcm_reject_msg(reason);

	if (rdma_protocol_iwarp(id->device, id->port_num))
		return iwcm_reject_msg(reason);

	WARN_ON_ONCE(1);
	return "unrecognized transport";
}