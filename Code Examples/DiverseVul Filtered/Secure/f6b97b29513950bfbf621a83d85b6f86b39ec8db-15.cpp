static struct sock *nr_find_peer(unsigned char index, unsigned char id,
	ax25_address *dest)
{
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&nr_list_lock);
	sk_for_each(s, node, &nr_list) {
		struct nr_sock *nr = nr_sk(s);

		if (nr->your_index == index && nr->your_id == id &&
		    !ax25cmp(&nr->dest_addr, dest)) {
			bh_lock_sock(s);
			goto found;
		}
	}
	s = NULL;
found:
	spin_unlock_bh(&nr_list_lock);
	return s;
}