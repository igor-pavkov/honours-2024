void qrtr_endpoint_unregister(struct qrtr_endpoint *ep)
{
	struct qrtr_node *node = ep->node;
	struct sockaddr_qrtr src = {AF_QIPCRTR, node->nid, QRTR_PORT_CTRL};
	struct sockaddr_qrtr dst = {AF_QIPCRTR, qrtr_local_nid, QRTR_PORT_CTRL};
	struct radix_tree_iter iter;
	struct qrtr_ctrl_pkt *pkt;
	struct qrtr_tx_flow *flow;
	struct sk_buff *skb;
	unsigned long flags;
	void __rcu **slot;

	mutex_lock(&node->ep_lock);
	node->ep = NULL;
	mutex_unlock(&node->ep_lock);

	/* Notify the local controller about the event */
	spin_lock_irqsave(&qrtr_nodes_lock, flags);
	radix_tree_for_each_slot(slot, &qrtr_nodes, &iter, 0) {
		if (*slot != node)
			continue;
		src.sq_node = iter.index;
		skb = qrtr_alloc_ctrl_packet(&pkt, GFP_ATOMIC);
		if (skb) {
			pkt->cmd = cpu_to_le32(QRTR_TYPE_BYE);
			qrtr_local_enqueue(NULL, skb, QRTR_TYPE_BYE, &src, &dst);
		}
	}
	spin_unlock_irqrestore(&qrtr_nodes_lock, flags);

	/* Wake up any transmitters waiting for resume-tx from the node */
	mutex_lock(&node->qrtr_tx_lock);
	radix_tree_for_each_slot(slot, &node->qrtr_tx_flow, &iter, 0) {
		flow = *slot;
		wake_up_interruptible_all(&flow->resume_tx);
	}
	mutex_unlock(&node->qrtr_tx_lock);

	qrtr_node_release(node);
	ep->node = NULL;
}