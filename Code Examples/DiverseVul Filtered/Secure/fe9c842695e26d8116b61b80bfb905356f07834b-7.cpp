int nfc_llcp_send_snl_sdres(struct nfc_llcp_local *local,
			    struct hlist_head *tlv_list, size_t tlvs_len)
{
	struct nfc_llcp_sdp_tlv *sdp;
	struct hlist_node *n;
	struct sk_buff *skb;

	skb = nfc_llcp_allocate_snl(local, tlvs_len);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	hlist_for_each_entry_safe(sdp, n, tlv_list, node) {
		skb_put_data(skb, sdp->tlv, sdp->tlv_len);

		hlist_del(&sdp->node);

		nfc_llcp_free_sdp_tlv(sdp);
	}

	skb_queue_tail(&local->tx_queue, skb);

	return 0;
}