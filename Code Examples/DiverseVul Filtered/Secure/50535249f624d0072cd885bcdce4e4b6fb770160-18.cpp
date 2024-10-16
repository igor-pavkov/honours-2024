static int qrtr_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct qrtr_sock *ipc = qrtr_sk(sock->sk);
	struct sock *sk = sock->sk;
	struct sockaddr_qrtr *sq;
	struct sk_buff *skb;
	struct ifreq ifr;
	long len = 0;
	int rc = 0;

	lock_sock(sk);

	switch (cmd) {
	case TIOCOUTQ:
		len = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (len < 0)
			len = 0;
		rc = put_user(len, (int __user *)argp);
		break;
	case TIOCINQ:
		skb = skb_peek(&sk->sk_receive_queue);
		if (skb)
			len = skb->len;
		rc = put_user(len, (int __user *)argp);
		break;
	case SIOCGIFADDR:
		if (copy_from_user(&ifr, argp, sizeof(ifr))) {
			rc = -EFAULT;
			break;
		}

		sq = (struct sockaddr_qrtr *)&ifr.ifr_addr;
		*sq = ipc->us;
		if (copy_to_user(argp, &ifr, sizeof(ifr))) {
			rc = -EFAULT;
			break;
		}
		break;
	case SIOCADDRT:
	case SIOCDELRT:
	case SIOCSIFADDR:
	case SIOCGIFDSTADDR:
	case SIOCSIFDSTADDR:
	case SIOCGIFBRDADDR:
	case SIOCSIFBRDADDR:
	case SIOCGIFNETMASK:
	case SIOCSIFNETMASK:
		rc = -EINVAL;
		break;
	default:
		rc = -ENOIOCTLCMD;
		break;
	}

	release_sock(sk);

	return rc;
}