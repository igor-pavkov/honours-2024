static void hidp_process_transmit(struct hidp_session *session,
				  struct sk_buff_head *transmit,
				  struct socket *sock)
{
	struct sk_buff *skb;
	int ret;

	BT_DBG("session %p", session);

	while ((skb = skb_dequeue(transmit))) {
		ret = hidp_send_frame(sock, skb->data, skb->len);
		if (ret == -EAGAIN) {
			skb_queue_head(transmit, skb);
			break;
		} else if (ret < 0) {
			hidp_session_terminate(session);
			kfree_skb(skb);
			break;
		}

		hidp_set_timer(session);
		kfree_skb(skb);
	}
}