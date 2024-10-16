static void cma_work_handler(struct work_struct *_work)
{
	struct cma_work *work = container_of(_work, struct cma_work, work);
	struct rdma_id_private *id_priv = work->id;

	mutex_lock(&id_priv->handler_mutex);
	if (READ_ONCE(id_priv->state) == RDMA_CM_DESTROYING ||
	    READ_ONCE(id_priv->state) == RDMA_CM_DEVICE_REMOVAL)
		goto out_unlock;
	if (work->old_state != 0 || work->new_state != 0) {
		if (!cma_comp_exch(id_priv, work->old_state, work->new_state))
			goto out_unlock;
	}

	if (cma_cm_event_handler(id_priv, &work->event)) {
		cma_id_put(id_priv);
		destroy_id_handler_unlock(id_priv);
		goto out_free;
	}

out_unlock:
	mutex_unlock(&id_priv->handler_mutex);
	cma_id_put(id_priv);
out_free:
	if (work->event.event == RDMA_CM_EVENT_MULTICAST_JOIN)
		rdma_destroy_ah_attr(&work->event.param.ud.ah_attr);
	kfree(work);
}