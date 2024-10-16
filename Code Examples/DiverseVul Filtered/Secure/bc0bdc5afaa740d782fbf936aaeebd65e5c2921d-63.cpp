static void cma_iboe_join_work_handler(struct work_struct *work)
{
	struct cma_multicast *mc =
		container_of(work, struct cma_multicast, iboe_join.work);
	struct rdma_cm_event *event = &mc->iboe_join.event;
	struct rdma_id_private *id_priv = mc->id_priv;
	int ret;

	mutex_lock(&id_priv->handler_mutex);
	if (READ_ONCE(id_priv->state) == RDMA_CM_DESTROYING ||
	    READ_ONCE(id_priv->state) == RDMA_CM_DEVICE_REMOVAL)
		goto out_unlock;

	ret = cma_cm_event_handler(id_priv, event);
	WARN_ON(ret);

out_unlock:
	mutex_unlock(&id_priv->handler_mutex);
	if (event->event == RDMA_CM_EVENT_MULTICAST_JOIN)
		rdma_destroy_ah_attr(&event->param.ud.ah_attr);
}