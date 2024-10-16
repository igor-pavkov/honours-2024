static struct qrtr_node *qrtr_node_lookup(unsigned int nid)
{
	struct qrtr_node *node;
	unsigned long flags;

	spin_lock_irqsave(&qrtr_nodes_lock, flags);
	node = radix_tree_lookup(&qrtr_nodes, nid);
	node = qrtr_node_acquire(node);
	spin_unlock_irqrestore(&qrtr_nodes_lock, flags);

	return node;
}