static void sk_enter_memory_pressure(struct sock *sk)
{
	if (!sk->sk_prot->enter_memory_pressure)
		return;

	sk->sk_prot->enter_memory_pressure(sk);
}