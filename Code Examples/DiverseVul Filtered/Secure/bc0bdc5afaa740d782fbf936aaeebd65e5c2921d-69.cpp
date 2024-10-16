int rdma_reject(struct rdma_cm_id *id, const void *private_data,
		u8 private_data_len, u8 reason)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!id_priv->cm_id.ib)
		return -EINVAL;

	if (rdma_cap_ib_cm(id->device, id->port_num)) {
		if (id->qp_type == IB_QPT_UD) {
			ret = cma_send_sidr_rep(id_priv, IB_SIDR_REJECT, 0,
						private_data, private_data_len);
		} else {
			trace_cm_send_rej(id_priv);
			ret = ib_send_cm_rej(id_priv->cm_id.ib, reason, NULL, 0,
					     private_data, private_data_len);
		}
	} else if (rdma_cap_iw_cm(id->device, id->port_num)) {
		ret = iw_cm_reject(id_priv->cm_id.iw,
				   private_data, private_data_len);
	} else {
		ret = -ENOSYS;
	}

	return ret;
}