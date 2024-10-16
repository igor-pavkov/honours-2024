static int qrtr_send_resume_tx(struct qrtr_cb *cb)
{
	struct sockaddr_qrtr remote = { AF_QIPCRTR, cb->src_node, cb->src_port };
	struct sockaddr_qrtr local = { AF_QIPCRTR, cb->dst_node, cb->dst_port };
	struct qrtr_ctrl_pkt *pkt;
	struct qrtr_node *node;
	struct sk_buff *skb;
	int ret;

	node = qrtr_node_lookup(remote.sq_node);
	if (!node)
		return -EINVAL;

	skb = qrtr_alloc_ctrl_packet(&pkt, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	pkt->cmd = cpu_to_le32(QRTR_TYPE_RESUME_TX);
	pkt->client.node = cpu_to_le32(cb->dst_node);
	pkt->client.port = cpu_to_le32(cb->dst_port);

	ret = qrtr_node_enqueue(node, skb, QRTR_TYPE_RESUME_TX, &local, &remote);

	qrtr_node_release(node);

	return ret;
}