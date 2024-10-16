void rdma_leave_multicast(struct rdma_cm_id *id, struct sockaddr *addr)
{
	struct rdma_id_private *id_priv;
	struct cma_multicast *mc;

	id_priv = container_of(id, struct rdma_id_private, id);
	spin_lock_irq(&id_priv->lock);
	list_for_each_entry(mc, &id_priv->mc_list, list) {
		if (memcmp(&mc->addr, addr, rdma_addr_size(addr)) != 0)
			continue;
		list_del(&mc->list);
		spin_unlock_irq(&id_priv->lock);

		WARN_ON(id_priv->cma_dev->device != id->device);
		destroy_mc(id_priv, mc);
		return;
	}
	spin_unlock_irq(&id_priv->lock);
}