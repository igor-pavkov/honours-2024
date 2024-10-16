static void rose_insert_socket(struct sock *sk)
{

	spin_lock_bh(&rose_list_lock);
	sk_add_node(sk, &rose_list);
	spin_unlock_bh(&rose_list_lock);
}