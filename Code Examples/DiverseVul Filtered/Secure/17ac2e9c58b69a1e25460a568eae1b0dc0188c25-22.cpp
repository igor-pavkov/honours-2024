static struct sock *rose_find_listener(rose_address *addr, ax25_address *call)
{
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&rose_list_lock);
	sk_for_each(s, node, &rose_list) {
		struct rose_sock *rose = rose_sk(s);

		if (!rosecmp(&rose->source_addr, addr) &&
		    !ax25cmp(&rose->source_call, call) &&
		    !rose->source_ndigis && s->sk_state == TCP_LISTEN)
			goto found;
	}

	sk_for_each(s, node, &rose_list) {
		struct rose_sock *rose = rose_sk(s);

		if (!rosecmp(&rose->source_addr, addr) &&
		    !ax25cmp(&rose->source_call, &null_ax25_address) &&
		    s->sk_state == TCP_LISTEN)
			goto found;
	}
	s = NULL;
found:
	spin_unlock_bh(&rose_list_lock);
	return s;
}