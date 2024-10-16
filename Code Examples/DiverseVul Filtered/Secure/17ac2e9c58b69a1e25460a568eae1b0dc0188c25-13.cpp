struct sock *rose_find_socket(unsigned int lci, struct rose_neigh *neigh)
{
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&rose_list_lock);
	sk_for_each(s, node, &rose_list) {
		struct rose_sock *rose = rose_sk(s);

		if (rose->lci == lci && rose->neighbour == neigh)
			goto found;
	}
	s = NULL;
found:
	spin_unlock_bh(&rose_list_lock);
	return s;
}