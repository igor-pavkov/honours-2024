static void ccid_deactivate(struct ccid_operations *ccid_ops)
{
	ccid_kmem_cache_destroy(ccid_ops->ccid_hc_tx_slab);
	ccid_ops->ccid_hc_tx_slab = NULL;
	ccid_kmem_cache_destroy(ccid_ops->ccid_hc_rx_slab);
	ccid_ops->ccid_hc_rx_slab = NULL;

	pr_info("CCID: Deactivated CCID %d (%s)\n",
		ccid_ops->ccid_id, ccid_ops->ccid_name);
}