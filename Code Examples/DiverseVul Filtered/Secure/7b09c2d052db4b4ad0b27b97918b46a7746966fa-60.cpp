static int fib6_new_sernum(struct net *net)
{
	int new, old;

	do {
		old = atomic_read(&net->ipv6.fib6_sernum);
		new = old < INT_MAX ? old + 1 : 1;
	} while (atomic_cmpxchg(&net->ipv6.fib6_sernum,
				old, new) != old);
	return new;
}