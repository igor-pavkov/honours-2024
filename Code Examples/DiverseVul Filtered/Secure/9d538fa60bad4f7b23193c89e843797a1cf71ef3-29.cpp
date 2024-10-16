void __sk_mem_reclaim(struct sock *sk, int amount)
{
	amount >>= SK_MEM_QUANTUM_SHIFT;
	sk->sk_forward_alloc -= amount << SK_MEM_QUANTUM_SHIFT;
	__sk_mem_reduce_allocated(sk, amount);
}