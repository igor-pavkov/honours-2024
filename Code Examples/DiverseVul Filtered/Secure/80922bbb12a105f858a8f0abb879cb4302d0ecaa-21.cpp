static int __init aun_udp_initialise(void)
{
	int error;
	struct sockaddr_in sin;

	skb_queue_head_init(&aun_queue);
	spin_lock_init(&aun_queue_lock);
	setup_timer(&ab_cleanup_timer, ab_cleanup, 0);
	ab_cleanup_timer.expires = jiffies + (HZ*2);
	add_timer(&ab_cleanup_timer);

	memset(&sin, 0, sizeof(sin));
	sin.sin_port = htons(AUN_PORT);

	/* We can count ourselves lucky Acorn machines are too dim to
	   speak IPv6. :-) */
	if ((error = sock_create_kern(PF_INET, SOCK_DGRAM, 0, &udpsock)) < 0)
	{
		printk("AUN: socket error %d\n", -error);
		return error;
	}

	udpsock->sk->sk_reuse = 1;
	udpsock->sk->sk_allocation = GFP_ATOMIC; /* we're going to call it
						    from interrupts */

	error = udpsock->ops->bind(udpsock, (struct sockaddr *)&sin,
				sizeof(sin));
	if (error < 0)
	{
		printk("AUN: bind error %d\n", -error);
		goto release;
	}

	udpsock->sk->sk_data_ready = aun_data_available;

	return 0;

release:
	sock_release(udpsock);
	udpsock = NULL;
	return error;
}