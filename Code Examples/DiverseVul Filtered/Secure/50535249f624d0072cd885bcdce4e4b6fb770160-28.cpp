static int qrtr_sendmsg(struct socket *sock, struct msghdr *msg, size_t len)
{
	DECLARE_SOCKADDR(struct sockaddr_qrtr *, addr, msg->msg_name);
	int (*enqueue_fn)(struct qrtr_node *, struct sk_buff *, int,
			  struct sockaddr_qrtr *, struct sockaddr_qrtr *);
	__le32 qrtr_type = cpu_to_le32(QRTR_TYPE_DATA);
	struct qrtr_sock *ipc = qrtr_sk(sock->sk);
	struct sock *sk = sock->sk;
	struct qrtr_node *node;
	struct sk_buff *skb;
	size_t plen;
	u32 type;
	int rc;

	if (msg->msg_flags & ~(MSG_DONTWAIT))
		return -EINVAL;

	if (len > 65535)
		return -EMSGSIZE;

	lock_sock(sk);

	if (addr) {
		if (msg->msg_namelen < sizeof(*addr)) {
			release_sock(sk);
			return -EINVAL;
		}

		if (addr->sq_family != AF_QIPCRTR) {
			release_sock(sk);
			return -EINVAL;
		}

		rc = qrtr_autobind(sock);
		if (rc) {
			release_sock(sk);
			return rc;
		}
	} else if (sk->sk_state == TCP_ESTABLISHED) {
		addr = &ipc->peer;
	} else {
		release_sock(sk);
		return -ENOTCONN;
	}

	node = NULL;
	if (addr->sq_node == QRTR_NODE_BCAST) {
		if (addr->sq_port != QRTR_PORT_CTRL &&
		    qrtr_local_nid != QRTR_NODE_BCAST) {
			release_sock(sk);
			return -ENOTCONN;
		}
		enqueue_fn = qrtr_bcast_enqueue;
	} else if (addr->sq_node == ipc->us.sq_node) {
		enqueue_fn = qrtr_local_enqueue;
	} else {
		node = qrtr_node_lookup(addr->sq_node);
		if (!node) {
			release_sock(sk);
			return -ECONNRESET;
		}
		enqueue_fn = qrtr_node_enqueue;
	}

	plen = (len + 3) & ~3;
	skb = sock_alloc_send_skb(sk, plen + QRTR_HDR_MAX_SIZE,
				  msg->msg_flags & MSG_DONTWAIT, &rc);
	if (!skb) {
		rc = -ENOMEM;
		goto out_node;
	}

	skb_reserve(skb, QRTR_HDR_MAX_SIZE);

	rc = memcpy_from_msg(skb_put(skb, len), msg, len);
	if (rc) {
		kfree_skb(skb);
		goto out_node;
	}

	if (ipc->us.sq_port == QRTR_PORT_CTRL) {
		if (len < 4) {
			rc = -EINVAL;
			kfree_skb(skb);
			goto out_node;
		}

		/* control messages already require the type as 'command' */
		skb_copy_bits(skb, 0, &qrtr_type, 4);
	}

	type = le32_to_cpu(qrtr_type);
	rc = enqueue_fn(node, skb, type, &ipc->us, addr);
	if (rc >= 0)
		rc = len;

out_node:
	qrtr_node_release(node);
	release_sock(sk);

	return rc;
}