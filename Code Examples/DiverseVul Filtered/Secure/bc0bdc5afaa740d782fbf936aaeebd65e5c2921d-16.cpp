static void cma_send_device_removal_put(struct rdma_id_private *id_priv)
{
	struct rdma_cm_event event = { .event = RDMA_CM_EVENT_DEVICE_REMOVAL };
	enum rdma_cm_state state;
	unsigned long flags;

	mutex_lock(&id_priv->handler_mutex);
	/* Record that we want to remove the device */
	spin_lock_irqsave(&id_priv->lock, flags);
	state = id_priv->state;
	if (state == RDMA_CM_DESTROYING || state == RDMA_CM_DEVICE_REMOVAL) {
		spin_unlock_irqrestore(&id_priv->lock, flags);
		mutex_unlock(&id_priv->handler_mutex);
		cma_id_put(id_priv);
		return;
	}
	id_priv->state = RDMA_CM_DEVICE_REMOVAL;
	spin_unlock_irqrestore(&id_priv->lock, flags);

	if (cma_cm_event_handler(id_priv, &event)) {
		/*
		 * At this point the ULP promises it won't call
		 * rdma_destroy_id() concurrently
		 */
		cma_id_put(id_priv);
		mutex_unlock(&id_priv->handler_mutex);
		trace_cm_id_destroy(id_priv);
		_destroy_id(id_priv, state);
		return;
	}
	mutex_unlock(&id_priv->handler_mutex);

	/*
	 * If this races with destroy then the thread that first assigns state
	 * to a destroying does the cancel.
	 */
	cma_cancel_operation(id_priv, state);
	cma_id_put(id_priv);
}