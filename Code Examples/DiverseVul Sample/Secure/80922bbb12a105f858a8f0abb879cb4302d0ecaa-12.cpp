static void econet_destroy_timer(unsigned long data)
{
	struct sock *sk=(struct sock *)data;

	if (!sk_has_allocations(sk)) {
		sk_free(sk);
		return;
	}

	sk->sk_timer.expires = jiffies + 10 * HZ;
	add_timer(&sk->sk_timer);
	printk(KERN_DEBUG "econet socket destroy delayed\n");
}