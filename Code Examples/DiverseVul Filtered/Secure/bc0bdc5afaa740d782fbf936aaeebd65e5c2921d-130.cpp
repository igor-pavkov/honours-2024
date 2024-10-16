static int cma_ib_req_handler(struct ib_cm_id *cm_id,
			      const struct ib_cm_event *ib_event)
{
	struct rdma_id_private *listen_id, *conn_id = NULL;
	struct rdma_cm_event event = {};
	struct cma_req_info req = {};
	struct net_device *net_dev;
	u8 offset;
	int ret;

	listen_id = cma_ib_id_from_event(cm_id, ib_event, &req, &net_dev);
	if (IS_ERR(listen_id))
		return PTR_ERR(listen_id);

	trace_cm_req_handler(listen_id, ib_event->event);
	if (!cma_ib_check_req_qp_type(&listen_id->id, ib_event)) {
		ret = -EINVAL;
		goto net_dev_put;
	}

	mutex_lock(&listen_id->handler_mutex);
	if (READ_ONCE(listen_id->state) != RDMA_CM_LISTEN) {
		ret = -ECONNABORTED;
		goto err_unlock;
	}

	offset = cma_user_data_offset(listen_id);
	event.event = RDMA_CM_EVENT_CONNECT_REQUEST;
	if (ib_event->event == IB_CM_SIDR_REQ_RECEIVED) {
		conn_id = cma_ib_new_udp_id(&listen_id->id, ib_event, net_dev);
		event.param.ud.private_data = ib_event->private_data + offset;
		event.param.ud.private_data_len =
				IB_CM_SIDR_REQ_PRIVATE_DATA_SIZE - offset;
	} else {
		conn_id = cma_ib_new_conn_id(&listen_id->id, ib_event, net_dev);
		cma_set_req_event_data(&event, &ib_event->param.req_rcvd,
				       ib_event->private_data, offset);
	}
	if (!conn_id) {
		ret = -ENOMEM;
		goto err_unlock;
	}

	mutex_lock_nested(&conn_id->handler_mutex, SINGLE_DEPTH_NESTING);
	ret = cma_ib_acquire_dev(conn_id, listen_id, &req);
	if (ret) {
		destroy_id_handler_unlock(conn_id);
		goto err_unlock;
	}

	conn_id->cm_id.ib = cm_id;
	cm_id->context = conn_id;
	cm_id->cm_handler = cma_ib_handler;

	ret = cma_cm_event_handler(conn_id, &event);
	if (ret) {
		/* Destroy the CM ID by returning a non-zero value. */
		conn_id->cm_id.ib = NULL;
		mutex_unlock(&listen_id->handler_mutex);
		destroy_id_handler_unlock(conn_id);
		goto net_dev_put;
	}

	if (READ_ONCE(conn_id->state) == RDMA_CM_CONNECT &&
	    conn_id->id.qp_type != IB_QPT_UD) {
		trace_cm_send_mra(cm_id->context);
		ib_send_cm_mra(cm_id, CMA_CM_MRA_SETTING, NULL, 0);
	}
	mutex_unlock(&conn_id->handler_mutex);

err_unlock:
	mutex_unlock(&listen_id->handler_mutex);

net_dev_put:
	if (net_dev)
		dev_put(net_dev);

	return ret;
}