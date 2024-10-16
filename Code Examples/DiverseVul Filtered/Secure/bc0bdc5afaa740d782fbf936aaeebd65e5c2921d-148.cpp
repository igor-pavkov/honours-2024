static int cma_ib_handler(struct ib_cm_id *cm_id,
			  const struct ib_cm_event *ib_event)
{
	struct rdma_id_private *id_priv = cm_id->context;
	struct rdma_cm_event event = {};
	enum rdma_cm_state state;
	int ret;

	mutex_lock(&id_priv->handler_mutex);
	state = READ_ONCE(id_priv->state);
	if ((ib_event->event != IB_CM_TIMEWAIT_EXIT &&
	     state != RDMA_CM_CONNECT) ||
	    (ib_event->event == IB_CM_TIMEWAIT_EXIT &&
	     state != RDMA_CM_DISCONNECT))
		goto out;

	switch (ib_event->event) {
	case IB_CM_REQ_ERROR:
	case IB_CM_REP_ERROR:
		event.event = RDMA_CM_EVENT_UNREACHABLE;
		event.status = -ETIMEDOUT;
		break;
	case IB_CM_REP_RECEIVED:
		if (state == RDMA_CM_CONNECT &&
		    (id_priv->id.qp_type != IB_QPT_UD)) {
			trace_cm_send_mra(id_priv);
			ib_send_cm_mra(cm_id, CMA_CM_MRA_SETTING, NULL, 0);
		}
		if (id_priv->id.qp) {
			event.status = cma_rep_recv(id_priv);
			event.event = event.status ? RDMA_CM_EVENT_CONNECT_ERROR :
						     RDMA_CM_EVENT_ESTABLISHED;
		} else {
			event.event = RDMA_CM_EVENT_CONNECT_RESPONSE;
		}
		cma_set_rep_event_data(&event, &ib_event->param.rep_rcvd,
				       ib_event->private_data);
		break;
	case IB_CM_RTU_RECEIVED:
	case IB_CM_USER_ESTABLISHED:
		event.event = RDMA_CM_EVENT_ESTABLISHED;
		break;
	case IB_CM_DREQ_ERROR:
		event.status = -ETIMEDOUT;
		fallthrough;
	case IB_CM_DREQ_RECEIVED:
	case IB_CM_DREP_RECEIVED:
		if (!cma_comp_exch(id_priv, RDMA_CM_CONNECT,
				   RDMA_CM_DISCONNECT))
			goto out;
		event.event = RDMA_CM_EVENT_DISCONNECTED;
		break;
	case IB_CM_TIMEWAIT_EXIT:
		event.event = RDMA_CM_EVENT_TIMEWAIT_EXIT;
		break;
	case IB_CM_MRA_RECEIVED:
		/* ignore event */
		goto out;
	case IB_CM_REJ_RECEIVED:
		pr_debug_ratelimited("RDMA CM: REJECTED: %s\n", rdma_reject_msg(&id_priv->id,
										ib_event->param.rej_rcvd.reason));
		cma_modify_qp_err(id_priv);
		event.status = ib_event->param.rej_rcvd.reason;
		event.event = RDMA_CM_EVENT_REJECTED;
		event.param.conn.private_data = ib_event->private_data;
		event.param.conn.private_data_len = IB_CM_REJ_PRIVATE_DATA_SIZE;
		break;
	default:
		pr_err("RDMA CMA: unexpected IB CM event: %d\n",
		       ib_event->event);
		goto out;
	}

	ret = cma_cm_event_handler(id_priv, &event);
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