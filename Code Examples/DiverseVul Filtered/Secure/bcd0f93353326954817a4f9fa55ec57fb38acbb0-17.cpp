static int pep_recvmsg(struct sock *sk, struct msghdr *msg, size_t len,
		       int noblock, int flags, int *addr_len)
{
	struct sk_buff *skb;
	int err;

	if (flags & ~(MSG_OOB|MSG_PEEK|MSG_TRUNC|MSG_DONTWAIT|MSG_WAITALL|
			MSG_NOSIGNAL|MSG_CMSG_COMPAT))
		return -EOPNOTSUPP;

	if (unlikely(1 << sk->sk_state & (TCPF_LISTEN | TCPF_CLOSE)))
		return -ENOTCONN;

	if ((flags & MSG_OOB) || sock_flag(sk, SOCK_URGINLINE)) {
		/* Dequeue and acknowledge control request */
		struct pep_sock *pn = pep_sk(sk);

		if (flags & MSG_PEEK)
			return -EOPNOTSUPP;
		skb = skb_dequeue(&pn->ctrlreq_queue);
		if (skb) {
			pep_ctrlreq_error(sk, skb, PN_PIPE_NO_ERROR,
						GFP_KERNEL);
			msg->msg_flags |= MSG_OOB;
			goto copy;
		}
		if (flags & MSG_OOB)
			return -EINVAL;
	}

	skb = skb_recv_datagram(sk, flags, noblock, &err);
	lock_sock(sk);
	if (skb == NULL) {
		if (err == -ENOTCONN && sk->sk_state == TCP_CLOSE_WAIT)
			err = -ECONNRESET;
		release_sock(sk);
		return err;
	}

	if (sk->sk_state == TCP_ESTABLISHED)
		pipe_grant_credits(sk, GFP_KERNEL);
	release_sock(sk);
copy:
	msg->msg_flags |= MSG_EOR;
	if (skb->len > len)
		msg->msg_flags |= MSG_TRUNC;
	else
		len = skb->len;

	err = skb_copy_datagram_msg(skb, 0, msg, len);
	if (!err)
		err = (flags & MSG_TRUNC) ? skb->len : len;

	skb_free_datagram(sk, skb);
	return err;
}