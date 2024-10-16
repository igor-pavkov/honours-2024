static int qrtr_bcast_enqueue(struct qrtr_node *node, struct sk_buff *skb,
			      int type, struct sockaddr_qrtr *from,
			      struct sockaddr_qrtr *to)
{
	struct sk_buff *skbn;

	mutex_lock(&qrtr_node_lock);
	list_for_each_entry(node, &qrtr_all_nodes, item) {
		skbn = skb_clone(skb, GFP_KERNEL);
		if (!skbn)
			break;
		skb_set_owner_w(skbn, skb->sk);
		qrtr_node_enqueue(node, skbn, type, from, to);
	}
	mutex_unlock(&qrtr_node_lock);

	qrtr_local_enqueue(NULL, skb, type, from, to);

	return 0;
}