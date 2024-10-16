static void sk_init_common(struct sock *sk)
{
	skb_queue_head_init(&sk->sk_receive_queue);
	skb_queue_head_init(&sk->sk_write_queue);
	skb_queue_head_init(&sk->sk_error_queue);

	rwlock_init(&sk->sk_callback_lock);
	lockdep_set_class_and_name(&sk->sk_receive_queue.lock,
			af_rlock_keys + sk->sk_family,
			af_family_rlock_key_strings[sk->sk_family]);
	lockdep_set_class_and_name(&sk->sk_write_queue.lock,
			af_wlock_keys + sk->sk_family,
			af_family_wlock_key_strings[sk->sk_family]);
	lockdep_set_class_and_name(&sk->sk_error_queue.lock,
			af_elock_keys + sk->sk_family,
			af_family_elock_key_strings[sk->sk_family]);
	lockdep_set_class_and_name(&sk->sk_callback_lock,
			af_callback_keys + sk->sk_family,
			af_family_clock_key_strings[sk->sk_family]);
}