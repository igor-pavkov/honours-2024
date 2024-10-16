static void qrtr_tx_resume(struct qrtr_node *node, struct sk_buff *skb)
{
	struct qrtr_ctrl_pkt *pkt = (struct qrtr_ctrl_pkt *)skb->data;
	u64 remote_node = le32_to_cpu(pkt->client.node);
	u32 remote_port = le32_to_cpu(pkt->client.port);
	struct qrtr_tx_flow *flow;
	unsigned long key;

	key = remote_node << 32 | remote_port;

	rcu_read_lock();
	flow = radix_tree_lookup(&node->qrtr_tx_flow, key);
	rcu_read_unlock();
	if (flow) {
		spin_lock(&flow->resume_tx.lock);
		flow->pending = 0;
		spin_unlock(&flow->resume_tx.lock);
		wake_up_interruptible_all(&flow->resume_tx);
	}

	consume_skb(skb);
}