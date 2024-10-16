static void _destroy_id(struct rdma_id_private *id_priv,
			enum rdma_cm_state state)
{
	cma_cancel_operation(id_priv, state);

	rdma_restrack_del(&id_priv->res);
	if (id_priv->cma_dev) {
		if (rdma_cap_ib_cm(id_priv->id.device, 1)) {
			if (id_priv->cm_id.ib)
				ib_destroy_cm_id(id_priv->cm_id.ib);
		} else if (rdma_cap_iw_cm(id_priv->id.device, 1)) {
			if (id_priv->cm_id.iw)
				iw_destroy_cm_id(id_priv->cm_id.iw);
		}
		cma_leave_mc_groups(id_priv);
		cma_release_dev(id_priv);
	}

	cma_release_port(id_priv);
	cma_id_put(id_priv);
	wait_for_completion(&id_priv->comp);

	if (id_priv->internal_id)
		cma_id_put(id_priv->id.context);

	kfree(id_priv->id.route.path_rec);

	put_net(id_priv->id.route.addr.dev_addr.net);
	kfree(id_priv);
}