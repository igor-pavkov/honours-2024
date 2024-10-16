void __sk_mem_reclaim(struct sock *sk)
{
	struct proto *prot = sk->sk_prot;

	atomic_sub(sk->sk_forward_alloc >> SK_MEM_QUANTUM_SHIFT,
		   prot->memory_allocated);
	sk->sk_forward_alloc &= SK_MEM_QUANTUM - 1;

	if (prot->memory_pressure && *prot->memory_pressure &&
	    (atomic_read(prot->memory_allocated) < prot->sysctl_mem[0]))
		*prot->memory_pressure = 0;
}