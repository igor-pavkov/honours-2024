void rose_destroy_socket(struct sock *sk)
{
	struct sk_buff *skb;

	rose_remove_socket(sk);
	rose_stop_heartbeat(sk);
	rose_stop_idletimer(sk);
	rose_stop_timer(sk);

	rose_clear_queues(sk);		/* Flush the queues */

	while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
		if (skb->sk != sk) {	/* A pending connection */
			/* Queue the unaccepted socket for death */
			sock_set_flag(skb->sk, SOCK_DEAD);
			rose_start_heartbeat(skb->sk);
			rose_sk(skb->sk)->state = ROSE_STATE_0;
		}

		kfree_skb(skb);
	}

	if (sk_has_allocations(sk)) {
		/* Defer: outstanding buffers */
		setup_timer(&sk->sk_timer, rose_destroy_timer,
				(unsigned long)sk);
		sk->sk_timer.expires  = jiffies + 10 * HZ;
		add_timer(&sk->sk_timer);
	} else
		sock_put(sk);
}