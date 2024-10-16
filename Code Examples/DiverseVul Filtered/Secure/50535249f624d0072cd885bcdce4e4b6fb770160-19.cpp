static void __qrtr_node_release(struct kref *kref)
{
	struct qrtr_node *node = container_of(kref, struct qrtr_node, ref);
	struct radix_tree_iter iter;
	struct qrtr_tx_flow *flow;
	unsigned long flags;
	void __rcu **slot;

	spin_lock_irqsave(&qrtr_nodes_lock, flags);
	/* If the node is a bridge for other nodes, there are possibly
	 * multiple entries pointing to our released node, delete them all.
	 */
	radix_tree_for_each_slot(slot, &qrtr_nodes, &iter, 0) {
		if (*slot == node)
			radix_tree_iter_delete(&qrtr_nodes, &iter, slot);
	}
	spin_unlock_irqrestore(&qrtr_nodes_lock, flags);

	list_del(&node->item);
	mutex_unlock(&qrtr_node_lock);

	skb_queue_purge(&node->rx_queue);

	/* Free tx flow counters */
	radix_tree_for_each_slot(slot, &node->qrtr_tx_flow, &iter, 0) {
		flow = *slot;
		radix_tree_iter_delete(&node->qrtr_tx_flow, &iter, slot);
		kfree(flow);
	}
	kfree(node);
}