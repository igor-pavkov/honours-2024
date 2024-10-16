static long caif_wait_for_flow_on(struct caifsock *cf_sk,
				int wait_writeable, long timeo, int *err)
{
	struct sock *sk = &cf_sk->sk;
	DEFINE_WAIT(wait);
	for (;;) {
		*err = 0;
		if (tx_flow_is_on(cf_sk) &&
			(!wait_writeable || sock_writeable(&cf_sk->sk)))
			break;
		*err = -ETIMEDOUT;
		if (!timeo)
			break;
		*err = -ERESTARTSYS;
		if (signal_pending(current))
			break;
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		*err = -ECONNRESET;
		if (sk->sk_shutdown & SHUTDOWN_MASK)
			break;
		*err = -sk->sk_err;
		if (sk->sk_err)
			break;
		*err = -EPIPE;
		if (cf_sk->sk.sk_state != CAIF_CONNECTED)
			break;
		timeo = schedule_timeout(timeo);
	}
	finish_wait(sk_sleep(sk), &wait);
	return timeo;
}