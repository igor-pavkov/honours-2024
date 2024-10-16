struct ccid *ccid_new(const u8 id, struct sock *sk, bool rx)
{
	struct ccid_operations *ccid_ops = ccid_by_number(id);
	struct ccid *ccid = NULL;

	if (ccid_ops == NULL)
		goto out;

	ccid = kmem_cache_alloc(rx ? ccid_ops->ccid_hc_rx_slab :
				     ccid_ops->ccid_hc_tx_slab, gfp_any());
	if (ccid == NULL)
		goto out;
	ccid->ccid_ops = ccid_ops;
	if (rx) {
		memset(ccid + 1, 0, ccid_ops->ccid_hc_rx_obj_size);
		if (ccid->ccid_ops->ccid_hc_rx_init != NULL &&
		    ccid->ccid_ops->ccid_hc_rx_init(ccid, sk) != 0)
			goto out_free_ccid;
	} else {
		memset(ccid + 1, 0, ccid_ops->ccid_hc_tx_obj_size);
		if (ccid->ccid_ops->ccid_hc_tx_init != NULL &&
		    ccid->ccid_ops->ccid_hc_tx_init(ccid, sk) != 0)
			goto out_free_ccid;
	}
out:
	return ccid;
out_free_ccid:
	kmem_cache_free(rx ? ccid_ops->ccid_hc_rx_slab :
			ccid_ops->ccid_hc_tx_slab, ccid);
	ccid = NULL;
	goto out;
}