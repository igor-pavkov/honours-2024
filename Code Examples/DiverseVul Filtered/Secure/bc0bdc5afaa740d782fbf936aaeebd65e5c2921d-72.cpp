static void cma_leave_mc_groups(struct rdma_id_private *id_priv)
{
	struct cma_multicast *mc;

	while (!list_empty(&id_priv->mc_list)) {
		mc = list_first_entry(&id_priv->mc_list, struct cma_multicast,
				      list);
		list_del(&mc->list);
		destroy_mc(id_priv, mc);
	}
}