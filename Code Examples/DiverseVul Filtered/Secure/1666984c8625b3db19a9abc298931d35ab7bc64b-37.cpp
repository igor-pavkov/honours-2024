static void __usbnet_queue_skb(struct sk_buff_head *list,
			struct sk_buff *newsk, enum skb_state state)
{
	struct skb_data *entry = (struct skb_data *) newsk->cb;

	__skb_queue_tail(list, newsk);
	entry->state = state;
}