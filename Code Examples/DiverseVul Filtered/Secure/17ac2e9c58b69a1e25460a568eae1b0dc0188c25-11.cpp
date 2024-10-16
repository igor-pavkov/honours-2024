static void rose_kill_by_device(struct net_device *dev)
{
	struct sock *s;
	struct hlist_node *node;

	spin_lock_bh(&rose_list_lock);
	sk_for_each(s, node, &rose_list) {
		struct rose_sock *rose = rose_sk(s);

		if (rose->device == dev) {
			rose_disconnect(s, ENETUNREACH, ROSE_OUT_OF_ORDER, 0);
			rose->neighbour->use--;
			rose->device = NULL;
		}
	}
	spin_unlock_bh(&rose_list_lock);
}