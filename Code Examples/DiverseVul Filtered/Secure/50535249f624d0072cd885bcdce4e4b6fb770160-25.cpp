static void qrtr_node_assign(struct qrtr_node *node, unsigned int nid)
{
	unsigned long flags;

	if (nid == QRTR_EP_NID_AUTO)
		return;

	spin_lock_irqsave(&qrtr_nodes_lock, flags);
	radix_tree_insert(&qrtr_nodes, nid, node);
	if (node->nid == QRTR_EP_NID_AUTO)
		node->nid = nid;
	spin_unlock_irqrestore(&qrtr_nodes_lock, flags);
}