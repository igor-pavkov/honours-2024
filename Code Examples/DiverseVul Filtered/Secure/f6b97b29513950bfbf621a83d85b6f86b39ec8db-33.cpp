static struct sock *nr_find_socket(unsigned char index, unsigned char id)
{
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&nr_list_lock);
	sk_for_each(s, node, &nr_list) {
		struct nr_sock *nr = nr_sk(s);

		if (nr->my_index == index && nr->my_id == id) {
			bh_lock_sock(s);
			goto found;
		}
	}
	s = NULL;
found:
	spin_unlock_bh(&nr_list_lock);
	return s;
}