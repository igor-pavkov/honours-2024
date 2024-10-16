static int llc_ui_getsockopt(struct socket *sock, int level, int optname,
			     char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct llc_sock *llc = llc_sk(sk);
	int val = 0, len = 0, rc = -EINVAL;

	lock_sock(sk);
	if (unlikely(level != SOL_LLC))
		goto out;
	rc = get_user(len, optlen);
	if (rc)
		goto out;
	rc = -EINVAL;
	if (len != sizeof(int))
		goto out;
	switch (optname) {
	case LLC_OPT_RETRY:
		val = llc->n2;					break;
	case LLC_OPT_SIZE:
		val = llc->n1;					break;
	case LLC_OPT_ACK_TMR_EXP:
		val = llc->ack_timer.expire / HZ;		break;
	case LLC_OPT_P_TMR_EXP:
		val = llc->pf_cycle_timer.expire / HZ;		break;
	case LLC_OPT_REJ_TMR_EXP:
		val = llc->rej_sent_timer.expire / HZ;		break;
	case LLC_OPT_BUSY_TMR_EXP:
		val = llc->busy_state_timer.expire / HZ;	break;
	case LLC_OPT_TX_WIN:
		val = llc->k;				break;
	case LLC_OPT_RX_WIN:
		val = llc->rw;				break;
	default:
		rc = -ENOPROTOOPT;
		goto out;
	}
	rc = 0;
	if (put_user(len, optlen) || copy_to_user(optval, &val, len))
		rc = -EFAULT;
out:
	release_sock(sk);
	return rc;
}