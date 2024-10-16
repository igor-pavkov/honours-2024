int nfc_llcp_send_i_frame(struct nfc_llcp_sock *sock,
			  struct msghdr *msg, size_t len)
{
	struct sk_buff *pdu;
	struct sock *sk = &sock->sk;
	struct nfc_llcp_local *local;
	size_t frag_len = 0, remaining_len;
	u8 *msg_data, *msg_ptr;
	u16 remote_miu;

	pr_debug("Send I frame len %zd\n", len);

	local = sock->local;
	if (local == NULL)
		return -ENODEV;

	/* Remote is ready but has not acknowledged our frames */
	if((sock->remote_ready &&
	    skb_queue_len(&sock->tx_pending_queue) >= sock->remote_rw &&
	    skb_queue_len(&sock->tx_queue) >= 2 * sock->remote_rw)) {
		pr_err("Pending queue is full %d frames\n",
		       skb_queue_len(&sock->tx_pending_queue));
		return -ENOBUFS;
	}

	/* Remote is not ready and we've been queueing enough frames */
	if ((!sock->remote_ready &&
	     skb_queue_len(&sock->tx_queue) >= 2 * sock->remote_rw)) {
		pr_err("Tx queue is full %d frames\n",
		       skb_queue_len(&sock->tx_queue));
		return -ENOBUFS;
	}

	msg_data = kmalloc(len, GFP_USER | __GFP_NOWARN);
	if (msg_data == NULL)
		return -ENOMEM;

	if (memcpy_from_msg(msg_data, msg, len)) {
		kfree(msg_data);
		return -EFAULT;
	}

	remaining_len = len;
	msg_ptr = msg_data;

	do {
		remote_miu = sock->remote_miu > LLCP_MAX_MIU ?
				LLCP_DEFAULT_MIU : sock->remote_miu;

		frag_len = min_t(size_t, remote_miu, remaining_len);

		pr_debug("Fragment %zd bytes remaining %zd",
			 frag_len, remaining_len);

		pdu = llcp_allocate_pdu(sock, LLCP_PDU_I,
					frag_len + LLCP_SEQUENCE_SIZE);
		if (pdu == NULL) {
			kfree(msg_data);
			return -ENOMEM;
		}

		skb_put(pdu, LLCP_SEQUENCE_SIZE);

		if (likely(frag_len > 0))
			skb_put_data(pdu, msg_ptr, frag_len);

		skb_queue_tail(&sock->tx_queue, pdu);

		lock_sock(sk);

		nfc_llcp_queue_i_frames(sock);

		release_sock(sk);

		remaining_len -= frag_len;
		msg_ptr += frag_len;
	} while (remaining_len > 0);

	kfree(msg_data);

	return len;
}