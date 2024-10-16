void rose_kill_by_neigh(struct rose_neigh *neigh)
{
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&rose_list_lock);
	sk_for_each(s, node, &rose_list) {
		struct rose_sock *rose = rose_sk(s);

		if (rose->neighbour == neigh) {
			rose_disconnect(s, ENETUNREACH, ROSE_OUT_OF_ORDER, 0);
			rose->neighbour->use--;
			rose->neighbour = NULL;
		}
	}
	spin_unlock_bh(&rose_list_lock);
}