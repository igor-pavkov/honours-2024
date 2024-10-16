static int peek_head_len(struct vhost_net_virtqueue *rvq, struct sock *sk)
{
	struct sk_buff *head;
	int len = 0;
	unsigned long flags;

	if (rvq->rx_ring)
		return vhost_net_buf_peek(rvq);

	spin_lock_irqsave(&sk->sk_receive_queue.lock, flags);
	head = skb_peek(&sk->sk_receive_queue);
	if (likely(head)) {
		len = head->len;
		if (skb_vlan_tag_present(head))
			len += VLAN_HLEN;
	}

	spin_unlock_irqrestore(&sk->sk_receive_queue.lock, flags);
	return len;
}