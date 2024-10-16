void sk_stop_timer(struct sock *sk, struct timer_list* timer)
{
	if (timer_pending(timer) && del_timer(timer))
		__sock_put(sk);
}