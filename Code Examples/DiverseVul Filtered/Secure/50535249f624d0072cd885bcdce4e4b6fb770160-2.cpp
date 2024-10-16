static struct qrtr_node *qrtr_node_acquire(struct qrtr_node *node)
{
	if (node)
		kref_get(&node->ref);
	return node;
}