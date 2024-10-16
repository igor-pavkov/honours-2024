static int caif_seqpkt_sendmsg(struct kiocb *kiocb, struct socket *sock,
			struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct caifsock *cf_sk = container_of(sk, struct caifsock, sk);
	int buffer_size;
	int ret = 0;
	struct sk_buff *skb = NULL;
	int noblock;
	long timeo;
	caif_assert(cf_sk);
	ret = sock_error(sk);
	if (ret)
		goto err;

	ret = -EOPNOTSUPP;
	if (msg->msg_flags&MSG_OOB)
		goto err;

	ret = -EOPNOTSUPP;
	if (msg->msg_namelen)
		goto err;

	ret = -EINVAL;
	if (unlikely(msg->msg_iov->iov_base == NULL))
		goto err;
	noblock = msg->msg_flags & MSG_DONTWAIT;

	timeo = sock_sndtimeo(sk, noblock);
	timeo = caif_wait_for_flow_on(container_of(sk, struct caifsock, sk),
				1, timeo, &ret);

	if (ret)
		goto err;
	ret = -EPIPE;
	if (cf_sk->sk.sk_state != CAIF_CONNECTED ||
		sock_flag(sk, SOCK_DEAD) ||
		(sk->sk_shutdown & RCV_SHUTDOWN))
		goto err;

	/* Error if trying to write more than maximum frame size. */
	ret = -EMSGSIZE;
	if (len > cf_sk->maxframe && cf_sk->sk.sk_protocol != CAIFPROTO_RFM)
		goto err;

	buffer_size = len + cf_sk->headroom + cf_sk->tailroom;

	ret = -ENOMEM;
	skb = sock_alloc_send_skb(sk, buffer_size, noblock, &ret);

	if (!skb || skb_tailroom(skb) < buffer_size)
		goto err;

	skb_reserve(skb, cf_sk->headroom);

	ret = memcpy_fromiovec(skb_put(skb, len), msg->msg_iov, len);

	if (ret)
		goto err;
	ret = transmit_skb(skb, cf_sk, noblock, timeo);
	if (ret < 0)
		/* skb is already freed */
		return ret;

	return len;
err:
	kfree_skb(skb);
	return ret;
}