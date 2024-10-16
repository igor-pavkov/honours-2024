int rdma_disconnect(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!id_priv->cm_id.ib)
		return -EINVAL;

	if (rdma_cap_ib_cm(id->device, id->port_num)) {
		ret = cma_modify_qp_err(id_priv);
		if (ret)
			goto out;
		/* Initiate or respond to a disconnect. */
		trace_cm_disconnect(id_priv);
		if (ib_send_cm_dreq(id_priv->cm_id.ib, NULL, 0)) {
			if (!ib_send_cm_drep(id_priv->cm_id.ib, NULL, 0))
				trace_cm_sent_drep(id_priv);
		} else {
			trace_cm_sent_dreq(id_priv);
		}
	} else if (rdma_cap_iw_cm(id->device, id->port_num)) {
		ret = iw_cm_disconnect(id_priv->cm_id.iw, 0);
	} else
		ret = -EINVAL;

out:
	return ret;
}