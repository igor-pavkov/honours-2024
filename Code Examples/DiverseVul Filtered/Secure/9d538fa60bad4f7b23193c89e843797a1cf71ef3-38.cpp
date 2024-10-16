void __sk_mem_reduce_allocated(struct sock *sk, int amount)
{
	sk_memory_allocated_sub(sk, amount);

	if (mem_cgroup_sockets_enabled && sk->sk_memcg)
		mem_cgroup_uncharge_skmem(sk->sk_memcg, amount);

	if (sk_under_memory_pressure(sk) &&
	    (sk_memory_allocated(sk) < sk_prot_mem_limits(sk, 0)))
		sk_leave_memory_pressure(sk);
}