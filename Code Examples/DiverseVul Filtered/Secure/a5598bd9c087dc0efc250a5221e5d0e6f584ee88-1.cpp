static int iucv_sock_sendmsg(struct kiocb *iocb, struct socket *sock,
			     struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct iucv_sock *iucv = iucv_sk(sk);
	struct sk_buff *skb;
	struct iucv_message txmsg;
	struct cmsghdr *cmsg;
	int cmsg_done;
	long timeo;
	char user_id[9];
	char appl_id[9];
	int err;
	int noblock = msg->msg_flags & MSG_DONTWAIT;

	err = sock_error(sk);
	if (err)
		return err;

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

	/* SOCK_SEQPACKET: we do not support segmented records */
	if (sk->sk_type == SOCK_SEQPACKET && !(msg->msg_flags & MSG_EOR))
		return -EOPNOTSUPP;

	lock_sock(sk);

	if (sk->sk_shutdown & SEND_SHUTDOWN) {
		err = -EPIPE;
		goto out;
	}

	/* Return if the socket is not in connected state */
	if (sk->sk_state != IUCV_CONNECTED) {
		err = -ENOTCONN;
		goto out;
	}

	/* initialize defaults */
	cmsg_done   = 0;	/* check for duplicate headers */
	txmsg.class = 0;

	/* iterate over control messages */
	for (cmsg = CMSG_FIRSTHDR(msg); cmsg;
		cmsg = CMSG_NXTHDR(msg, cmsg)) {

		if (!CMSG_OK(msg, cmsg)) {
			err = -EINVAL;
			goto out;
		}

		if (cmsg->cmsg_level != SOL_IUCV)
			continue;

		if (cmsg->cmsg_type & cmsg_done) {
			err = -EINVAL;
			goto out;
		}
		cmsg_done |= cmsg->cmsg_type;

		switch (cmsg->cmsg_type) {
		case SCM_IUCV_TRGCLS:
			if (cmsg->cmsg_len != CMSG_LEN(TRGCLS_SIZE)) {
				err = -EINVAL;
				goto out;
			}

			/* set iucv message target class */
			memcpy(&txmsg.class,
				(void *) CMSG_DATA(cmsg), TRGCLS_SIZE);

			break;

		default:
			err = -EINVAL;
			goto out;
			break;
		}
	}

	/* allocate one skb for each iucv message:
	 * this is fine for SOCK_SEQPACKET (unless we want to support
	 * segmented records using the MSG_EOR flag), but
	 * for SOCK_STREAM we might want to improve it in future */
	if (iucv->transport == AF_IUCV_TRANS_HIPER)
		skb = sock_alloc_send_skb(sk,
			len + sizeof(struct af_iucv_trans_hdr) + ETH_HLEN,
			noblock, &err);
	else
		skb = sock_alloc_send_skb(sk, len, noblock, &err);
	if (!skb) {
		err = -ENOMEM;
		goto out;
	}
	if (iucv->transport == AF_IUCV_TRANS_HIPER)
		skb_reserve(skb, sizeof(struct af_iucv_trans_hdr) + ETH_HLEN);
	if (memcpy_fromiovec(skb_put(skb, len), msg->msg_iov, len)) {
		err = -EFAULT;
		goto fail;
	}

	/* wait if outstanding messages for iucv path has reached */
	timeo = sock_sndtimeo(sk, noblock);
	err = iucv_sock_wait(sk, iucv_below_msglim(sk), timeo);
	if (err)
		goto fail;

	/* return -ECONNRESET if the socket is no longer connected */
	if (sk->sk_state != IUCV_CONNECTED) {
		err = -ECONNRESET;
		goto fail;
	}

	/* increment and save iucv message tag for msg_completion cbk */
	txmsg.tag = iucv->send_tag++;
	memcpy(CB_TAG(skb), &txmsg.tag, CB_TAG_LEN);

	if (iucv->transport == AF_IUCV_TRANS_HIPER) {
		atomic_inc(&iucv->msg_sent);
		err = afiucv_hs_send(&txmsg, sk, skb, 0);
		if (err) {
			atomic_dec(&iucv->msg_sent);
			goto fail;
		}
		goto release;
	}
	skb_queue_tail(&iucv->send_skb_q, skb);

	if (((iucv->path->flags & IUCV_IPRMDATA) & iucv->flags)
	      && skb->len <= 7) {
		err = iucv_send_iprm(iucv->path, &txmsg, skb);

		/* on success: there is no message_complete callback
		 * for an IPRMDATA msg; remove skb from send queue */
		if (err == 0) {
			skb_unlink(skb, &iucv->send_skb_q);
			kfree_skb(skb);
		}

		/* this error should never happen since the
		 * IUCV_IPRMDATA path flag is set... sever path */
		if (err == 0x15) {
			pr_iucv->path_sever(iucv->path, NULL);
			skb_unlink(skb, &iucv->send_skb_q);
			err = -EPIPE;
			goto fail;
		}
	} else
		err = pr_iucv->message_send(iucv->path, &txmsg, 0, 0,
					(void *) skb->data, skb->len);
	if (err) {
		if (err == 3) {
			user_id[8] = 0;
			memcpy(user_id, iucv->dst_user_id, 8);
			appl_id[8] = 0;
			memcpy(appl_id, iucv->dst_name, 8);
			pr_err("Application %s on z/VM guest %s"
				" exceeds message limit\n",
				appl_id, user_id);
			err = -EAGAIN;
		} else
			err = -EPIPE;
		skb_unlink(skb, &iucv->send_skb_q);
		goto fail;
	}

release:
	release_sock(sk);
	return len;

fail:
	kfree_skb(skb);
out:
	release_sock(sk);
	return err;
}