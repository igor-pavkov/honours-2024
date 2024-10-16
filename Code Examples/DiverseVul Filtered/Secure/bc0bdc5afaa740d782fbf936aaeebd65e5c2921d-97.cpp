static int cma_listen_handler(struct rdma_cm_id *id,
			      struct rdma_cm_event *event)
{
	struct rdma_id_private *id_priv = id->context;

	/* Listening IDs are always destroyed on removal */
	if (event->event == RDMA_CM_EVENT_DEVICE_REMOVAL)
		return -1;

	id->context = id_priv->id.context;
	id->event_handler = id_priv->id.event_handler;
	trace_cm_event_handler(id_priv, event);
	return id_priv->id.event_handler(id, event);
}