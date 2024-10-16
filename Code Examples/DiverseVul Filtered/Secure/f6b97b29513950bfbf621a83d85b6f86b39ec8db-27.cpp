static void nr_remove_socket(struct sock *sk)
{
	spin_lock_bh(&nr_list_lock);
	sk_del_node_init(sk);
	spin_unlock_bh(&nr_list_lock);
}