void ccid_hc_rx_delete(struct ccid *ccid, struct sock *sk)
{
	if (ccid != NULL) {
		if (ccid->ccid_ops->ccid_hc_rx_exit != NULL)
			ccid->ccid_ops->ccid_hc_rx_exit(sk);
		kmem_cache_free(ccid->ccid_ops->ccid_hc_rx_slab, ccid);
	}
}