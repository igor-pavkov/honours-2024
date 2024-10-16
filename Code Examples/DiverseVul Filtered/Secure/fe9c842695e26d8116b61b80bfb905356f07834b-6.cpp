int nfc_llcp_send_snl_sdreq(struct nfc_llcp_local *local,
			    struct hlist_head *tlv_list, size_t tlvs_len)
{
	struct nfc_llcp_sdp_tlv *sdreq;
	struct hlist_node *n;
	struct sk_buff *skb;

	skb = nfc_llcp_allocate_snl(local, tlvs_len);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	mutex_lock(&local->sdreq_lock);

	if (hlist_empty(&local->pending_sdreqs))
		mod_timer(&local->sdreq_timer,
			  jiffies + msecs_to_jiffies(3 * local->remote_lto));

	hlist_for_each_entry_safe(sdreq, n, tlv_list, node) {
		pr_debug("tid %d for %s\n", sdreq->tid, sdreq->uri);

		skb_put_data(skb, sdreq->tlv, sdreq->tlv_len);

		hlist_del(&sdreq->node);

		hlist_add_head(&sdreq->node, &local->pending_sdreqs);
	}

	mutex_unlock(&local->sdreq_lock);

	skb_queue_tail(&local->tx_queue, skb);

	return 0;
}