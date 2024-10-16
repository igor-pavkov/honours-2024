static int cma_comp_exch(struct rdma_id_private *id_priv,
			 enum rdma_cm_state comp, enum rdma_cm_state exch)
{
	unsigned long flags;
	int ret;

	/*
	 * The FSM uses a funny double locking where state is protected by both
	 * the handler_mutex and the spinlock. State is not allowed to change
	 * to/from a handler_mutex protected value without also holding
	 * handler_mutex.
	 */
	if (comp == RDMA_CM_CONNECT || exch == RDMA_CM_CONNECT)
		lockdep_assert_held(&id_priv->handler_mutex);

	spin_lock_irqsave(&id_priv->lock, flags);
	if ((ret = (id_priv->state == comp)))
		id_priv->state = exch;
	spin_unlock_irqrestore(&id_priv->lock, flags);
	return ret;
}