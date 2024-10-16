static int cma_ib_mc_handler(int status, struct ib_sa_multicast *multicast)
{
	struct cma_multicast *mc = multicast->context;
	struct rdma_id_private *id_priv = mc->id_priv;
	struct rdma_cm_event event = {};
	int ret = 0;

	mutex_lock(&id_priv->handler_mutex);
	if (READ_ONCE(id_priv->state) == RDMA_CM_DEVICE_REMOVAL ||
	    READ_ONCE(id_priv->state) == RDMA_CM_DESTROYING)
		goto out;

	cma_make_mc_event(status, id_priv, multicast, &event, mc);
	ret = cma_cm_event_handler(id_priv, &event);
	rdma_destroy_ah_attr(&event.param.ud.ah_attr);
	WARN_ON(ret);

out:
	mutex_unlock(&id_priv->handler_mutex);
	return 0;
}