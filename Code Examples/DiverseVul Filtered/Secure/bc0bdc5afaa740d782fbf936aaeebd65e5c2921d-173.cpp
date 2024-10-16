static bool rdma_is_consumer_reject(struct rdma_cm_id *id, int reason)
{
	if (rdma_ib_or_roce(id->device, id->port_num))
		return reason == IB_CM_REJ_CONSUMER_DEFINED;

	if (rdma_protocol_iwarp(id->device, id->port_num))
		return reason == -ECONNREFUSED;

	WARN_ON_ONCE(1);
	return false;
}