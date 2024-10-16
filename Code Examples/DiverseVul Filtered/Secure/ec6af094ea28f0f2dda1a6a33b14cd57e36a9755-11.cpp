static noinline struct sk_buff *nf_hook_direct_egress(struct sk_buff *skb)
{
	struct sk_buff *next, *head = NULL, *tail;
	int rc;

	rcu_read_lock();
	for (; skb != NULL; skb = next) {
		next = skb->next;
		skb_mark_not_on_list(skb);

		if (!nf_hook_egress(skb, &rc, skb->dev))
			continue;

		if (!head)
			head = skb;
		else
			tail->next = skb;

		tail = skb;
	}
	rcu_read_unlock();

	return head;
}