static int qrtr_node_enqueue(struct qrtr_node *node, struct sk_buff *skb,
			     int type, struct sockaddr_qrtr *from,
			     struct sockaddr_qrtr *to)
{
	struct qrtr_hdr_v1 *hdr;
	size_t len = skb->len;
	int rc, confirm_rx;

	confirm_rx = qrtr_tx_wait(node, to->sq_node, to->sq_port, type);
	if (confirm_rx < 0) {
		kfree_skb(skb);
		return confirm_rx;
	}

	hdr = skb_push(skb, sizeof(*hdr));
	hdr->version = cpu_to_le32(QRTR_PROTO_VER_1);
	hdr->type = cpu_to_le32(type);
	hdr->src_node_id = cpu_to_le32(from->sq_node);
	hdr->src_port_id = cpu_to_le32(from->sq_port);
	if (to->sq_port == QRTR_PORT_CTRL) {
		hdr->dst_node_id = cpu_to_le32(node->nid);
		hdr->dst_port_id = cpu_to_le32(QRTR_PORT_CTRL);
	} else {
		hdr->dst_node_id = cpu_to_le32(to->sq_node);
		hdr->dst_port_id = cpu_to_le32(to->sq_port);
	}

	hdr->size = cpu_to_le32(len);
	hdr->confirm_rx = !!confirm_rx;

	rc = skb_put_padto(skb, ALIGN(len, 4) + sizeof(*hdr));

	if (!rc) {
		mutex_lock(&node->ep_lock);
		rc = -ENODEV;
		if (node->ep)
			rc = node->ep->xmit(node->ep, skb);
		else
			kfree_skb(skb);
		mutex_unlock(&node->ep_lock);
	}
	/* Need to ensure that a subsequent message carries the otherwise lost
	 * confirm_rx flag if we dropped this one */
	if (rc && confirm_rx)
		qrtr_tx_flow_failed(node, to->sq_node, to->sq_port);

	return rc;
}