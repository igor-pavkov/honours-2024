int __sk_mem_schedule(struct sock *sk, int size, int kind)
{
	int ret, amt = sk_mem_pages(size);

	sk->sk_forward_alloc += amt << SK_MEM_QUANTUM_SHIFT;
	ret = __sk_mem_raise_allocated(sk, size, amt, kind);
	if (!ret)
		sk->sk_forward_alloc -= amt << SK_MEM_QUANTUM_SHIFT;
	return ret;
}