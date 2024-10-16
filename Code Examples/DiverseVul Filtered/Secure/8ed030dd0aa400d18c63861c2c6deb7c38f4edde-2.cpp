static int ccid_activate(struct ccid_operations *ccid_ops)
{
	int err = -ENOBUFS;

	ccid_ops->ccid_hc_rx_slab =
			ccid_kmem_cache_create(ccid_ops->ccid_hc_rx_obj_size,
					       ccid_ops->ccid_hc_rx_slab_name,
					       "ccid%u_hc_rx_sock",
					       ccid_ops->ccid_id);
	if (ccid_ops->ccid_hc_rx_slab == NULL)
		goto out;

	ccid_ops->ccid_hc_tx_slab =
			ccid_kmem_cache_create(ccid_ops->ccid_hc_tx_obj_size,
					       ccid_ops->ccid_hc_tx_slab_name,
					       "ccid%u_hc_tx_sock",
					       ccid_ops->ccid_id);
	if (ccid_ops->ccid_hc_tx_slab == NULL)
		goto out_free_rx_slab;

	pr_info("CCID: Activated CCID %d (%s)\n",
		ccid_ops->ccid_id, ccid_ops->ccid_name);
	err = 0;
out:
	return err;
out_free_rx_slab:
	ccid_kmem_cache_destroy(ccid_ops->ccid_hc_rx_slab);
	ccid_ops->ccid_hc_rx_slab = NULL;
	goto out;
}