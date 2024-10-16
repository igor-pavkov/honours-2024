static void nr_insert_socket(struct sock *sk)
{
	spin_lock_bh(&nr_list_lock);
	sk_add_node(sk, &nr_list);
	spin_unlock_bh(&nr_list_lock);
}