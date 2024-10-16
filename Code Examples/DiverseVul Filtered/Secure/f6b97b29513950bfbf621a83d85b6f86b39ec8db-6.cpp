static struct sock *nr_find_listener(ax25_address *addr)
{
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&nr_list_lock);
	sk_for_each(s, node, &nr_list)
		if (!ax25cmp(&nr_sk(s)->source_addr, addr) &&
		    s->sk_state == TCP_LISTEN) {
			bh_lock_sock(s);
			goto found;
		}
	s = NULL;
found:
	spin_unlock_bh(&nr_list_lock);
	return s;
}