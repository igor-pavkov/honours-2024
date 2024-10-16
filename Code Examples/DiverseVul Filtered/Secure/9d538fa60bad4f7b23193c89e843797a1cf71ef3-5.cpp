static inline void sock_lock_init(struct sock *sk)
{
	if (sk->sk_kern_sock)
		sock_lock_init_class_and_name(
			sk,
			af_family_kern_slock_key_strings[sk->sk_family],
			af_family_kern_slock_keys + sk->sk_family,
			af_family_kern_key_strings[sk->sk_family],
			af_family_kern_keys + sk->sk_family);
	else
		sock_lock_init_class_and_name(
			sk,
			af_family_slock_key_strings[sk->sk_family],
			af_family_slock_keys + sk->sk_family,
			af_family_key_strings[sk->sk_family],
			af_family_keys + sk->sk_family);
}