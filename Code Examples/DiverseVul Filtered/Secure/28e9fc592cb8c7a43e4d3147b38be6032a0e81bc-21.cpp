static int llc_ui_wait_for_disc(struct sock *sk, long timeout)
{
	DEFINE_WAIT(wait);
	int rc = 0;

	while (1) {
		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		if (sk_wait_event(sk, &timeout, sk->sk_state == TCP_CLOSE))
			break;
		rc = -ERESTARTSYS;
		if (signal_pending(current))
			break;
		rc = -EAGAIN;
		if (!timeout)
			break;
		rc = 0;
	}
	finish_wait(sk->sk_sleep, &wait);
	return rc;
}