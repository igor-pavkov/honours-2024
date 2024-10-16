static int llc_ui_wait_for_busy_core(struct sock *sk, long timeout)
{
	DEFINE_WAIT(wait);
	struct llc_sock *llc = llc_sk(sk);
	int rc;

	while (1) {
		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		rc = 0;
		if (sk_wait_event(sk, &timeout,
				  (sk->sk_shutdown & RCV_SHUTDOWN) ||
				  (!llc_data_accept_state(llc->state) &&
				   !llc->remote_busy_flag &&
				   !llc->p_flag)))
			break;
		rc = -ERESTARTSYS;
		if (signal_pending(current))
			break;
		rc = -EAGAIN;
		if (!timeout)
			break;
	}
	finish_wait(sk->sk_sleep, &wait);
	return rc;
}