static int iw_conn_req_handler(struct iw_cm_id *cm_id,
			       struct iw_cm_event *iw_event)
{
	struct rdma_id_private *listen_id, *conn_id;
	struct rdma_cm_event event = {};
	int ret = -ECONNABORTED;
	struct sockaddr *laddr = (struct sockaddr *)&iw_event->local_addr;
	struct sockaddr *raddr = (struct sockaddr *)&iw_event->remote_addr;

	event.event = RDMA_CM_EVENT_CONNECT_REQUEST;
	event.param.conn.private_data = iw_event->private_data;
	event.param.conn.private_data_len = iw_event->private_data_len;
	event.param.conn.initiator_depth = iw_event->ird;
	event.param.conn.responder_resources = iw_event->ord;

	listen_id = cm_id->context;

	mutex_lock(&listen_id->handler_mutex);
	if (READ_ONCE(listen_id->state) != RDMA_CM_LISTEN)
		goto out;

	/* Create a new RDMA id for the new IW CM ID */
	conn_id = __rdma_create_id(listen_id->id.route.addr.dev_addr.net,
				   listen_id->id.event_handler,
				   listen_id->id.context, RDMA_PS_TCP,
				   IB_QPT_RC, listen_id);
	if (IS_ERR(conn_id)) {
		ret = -ENOMEM;
		goto out;
	}
	mutex_lock_nested(&conn_id->handler_mutex, SINGLE_DEPTH_NESTING);
	conn_id->state = RDMA_CM_CONNECT;

	ret = rdma_translate_ip(laddr, &conn_id->id.route.addr.dev_addr);
	if (ret) {
		mutex_unlock(&listen_id->handler_mutex);
		destroy_id_handler_unlock(conn_id);
		return ret;
	}

	ret = cma_iw_acquire_dev(conn_id, listen_id);
	if (ret) {
		mutex_unlock(&listen_id->handler_mutex);
		destroy_id_handler_unlock(conn_id);
		return ret;
	}

	conn_id->cm_id.iw = cm_id;
	cm_id->context = conn_id;
	cm_id->cm_handler = cma_iw_handler;

	memcpy(cma_src_addr(conn_id), laddr, rdma_addr_size(laddr));
	memcpy(cma_dst_addr(conn_id), raddr, rdma_addr_size(raddr));

	ret = cma_cm_event_handler(conn_id, &event);
	if (ret) {
		/* User wants to destroy the CM ID */
		conn_id->cm_id.iw = NULL;
		mutex_unlock(&listen_id->handler_mutex);
		destroy_id_handler_unlock(conn_id);
		return ret;
	}

	mutex_unlock(&conn_id->handler_mutex);

out:
	mutex_unlock(&listen_id->handler_mutex);
	return ret;
}