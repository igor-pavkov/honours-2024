void nr_destroy_socket(struct sock *sk)
{
	struct sk_buff *skb;

	nr_remove_socket(sk);

	nr_stop_heartbeat(sk);
	nr_stop_t1timer(sk);
	nr_stop_t2timer(sk);
	nr_stop_t4timer(sk);
	nr_stop_idletimer(sk);

	nr_clear_queues(sk);		/* Flush the queues */

	while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
		if (skb->sk != sk) { /* A pending connection */
			/* Queue the unaccepted socket for death */
			sock_set_flag(skb->sk, SOCK_DEAD);
			nr_start_heartbeat(skb->sk);
			nr_sk(skb->sk)->state = NR_STATE_0;
		}

		kfree_skb(skb);
	}

	if (sk_has_allocations(sk)) {
		/* Defer: outstanding buffers */
		sk->sk_timer.function = nr_destroy_timer;
		sk->sk_timer.expires  = jiffies + 2 * HZ;
		add_timer(&sk->sk_timer);
	} else
		sock_put(sk);
}