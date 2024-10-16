static void qrtr_tx_flow_failed(struct qrtr_node *node, int dest_node,
				int dest_port)
{
	unsigned long key = (u64)dest_node << 32 | dest_port;
	struct qrtr_tx_flow *flow;

	rcu_read_lock();
	flow = radix_tree_lookup(&node->qrtr_tx_flow, key);
	rcu_read_unlock();
	if (flow) {
		spin_lock_irq(&flow->resume_tx.lock);
		flow->tx_failed = 1;
		spin_unlock_irq(&flow->resume_tx.lock);
	}
}