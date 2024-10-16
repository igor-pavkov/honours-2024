static int llc_ui_setsockopt(struct socket *sock, int level, int optname,
			     char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct llc_sock *llc = llc_sk(sk);
	unsigned int opt;
	int rc = -EINVAL;

	lock_sock(sk);
	if (unlikely(level != SOL_LLC || optlen != sizeof(int)))
		goto out;
	rc = get_user(opt, (int __user *)optval);
	if (rc)
		goto out;
	rc = -EINVAL;
	switch (optname) {
	case LLC_OPT_RETRY:
		if (opt > LLC_OPT_MAX_RETRY)
			goto out;
		llc->n2 = opt;
		break;
	case LLC_OPT_SIZE:
		if (opt > LLC_OPT_MAX_SIZE)
			goto out;
		llc->n1 = opt;
		break;
	case LLC_OPT_ACK_TMR_EXP:
		if (opt > LLC_OPT_MAX_ACK_TMR_EXP)
			goto out;
		llc->ack_timer.expire = opt * HZ;
		break;
	case LLC_OPT_P_TMR_EXP:
		if (opt > LLC_OPT_MAX_P_TMR_EXP)
			goto out;
		llc->pf_cycle_timer.expire = opt * HZ;
		break;
	case LLC_OPT_REJ_TMR_EXP:
		if (opt > LLC_OPT_MAX_REJ_TMR_EXP)
			goto out;
		llc->rej_sent_timer.expire = opt * HZ;
		break;
	case LLC_OPT_BUSY_TMR_EXP:
		if (opt > LLC_OPT_MAX_BUSY_TMR_EXP)
			goto out;
		llc->busy_state_timer.expire = opt * HZ;
		break;
	case LLC_OPT_TX_WIN:
		if (opt > LLC_OPT_MAX_WIN)
			goto out;
		llc->k = opt;
		break;
	case LLC_OPT_RX_WIN:
		if (opt > LLC_OPT_MAX_WIN)
			goto out;
		llc->rw = opt;
		break;
	case LLC_OPT_PKTINFO:
		if (opt)
			llc->cmsg_flags |= LLC_CMSG_PKTINFO;
		else
			llc->cmsg_flags &= ~LLC_CMSG_PKTINFO;
		break;
	default:
		rc = -ENOPROTOOPT;
		goto out;
	}
	rc = 0;
out:
	release_sock(sk);
	return rc;
}