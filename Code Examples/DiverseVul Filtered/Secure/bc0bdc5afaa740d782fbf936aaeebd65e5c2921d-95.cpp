static int cma_sidr_rep_handler(struct ib_cm_id *cm_id,
				const struct ib_cm_event *ib_event)
{
	struct rdma_id_private *id_priv = cm_id->context;
	struct rdma_cm_event event = {};
	const struct ib_cm_sidr_rep_event_param *rep =
				&ib_event->param.sidr_rep_rcvd;
	int ret;

	mutex_lock(&id_priv->handler_mutex);
	if (READ_ONCE(id_priv->state) != RDMA_CM_CONNECT)
		goto out;

	switch (ib_event->event) {
	case IB_CM_SIDR_REQ_ERROR:
		event.event = RDMA_CM_EVENT_UNREACHABLE;
		event.status = -ETIMEDOUT;
		break;
	case IB_CM_SIDR_REP_RECEIVED:
		event.param.ud.private_data = ib_event->private_data;
		event.param.ud.private_data_len = IB_CM_SIDR_REP_PRIVATE_DATA_SIZE;
		if (rep->status != IB_SIDR_SUCCESS) {
			event.event = RDMA_CM_EVENT_UNREACHABLE;
			event.status = ib_event->param.sidr_rep_rcvd.status;
			pr_debug_ratelimited("RDMA CM: UNREACHABLE: bad SIDR reply. status %d\n",
					     event.status);
			break;
		}
		ret = cma_set_qkey(id_priv, rep->qkey);
		if (ret) {
			pr_debug_ratelimited("RDMA CM: ADDR_ERROR: failed to set qkey. status %d\n", ret);
			event.event = RDMA_CM_EVENT_ADDR_ERROR;
			event.status = ret;
			break;
		}
		ib_init_ah_attr_from_path(id_priv->id.device,
					  id_priv->id.port_num,
					  id_priv->id.route.path_rec,
					  &event.param.ud.ah_attr,
					  rep->sgid_attr);
		event.param.ud.qp_num = rep->qpn;
		event.param.ud.qkey = rep->qkey;
		event.event = RDMA_CM_EVENT_ESTABLISHED;
		event.status = 0;
		break;
	default:
		pr_err("RDMA CMA: unexpected IB CM event: %d\n",
		       ib_event->event);
		goto out;
	}

	ret = cma_cm_event_handler(id_priv, &event);

	rdma_destroy_ah_attr(&event.param.ud.ah_attr);
	if (ret) {
		/* Destroy the CM ID by returning a non-zero value. */
		id_priv->cm_id.ib = NULL;
		destroy_id_handler_unlock(id_priv);
		return ret;
	}
out:
	mutex_unlock(&id_priv->handler_mutex);
	return 0;
}