static void rose_remove_socket(struct sock *sk)
{
	spin_lock_bh(&rose_list_lock);
	sk_del_node_init(sk);
	spin_unlock_bh(&rose_list_lock);
}