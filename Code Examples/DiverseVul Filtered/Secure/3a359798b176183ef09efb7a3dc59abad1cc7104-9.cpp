static int llcp_sock_sendmsg(struct socket *sock, struct msghdr *msg,
			     size_t len)
{
	struct sock *sk = sock->sk;
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);
	int ret;

	pr_debug("sock %p sk %p", sock, sk);

	ret = sock_error(sk);
	if (ret)
		return ret;

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

	lock_sock(sk);

	if (sk->sk_type == SOCK_DGRAM) {
		DECLARE_SOCKADDR(struct sockaddr_nfc_llcp *, addr,
				 msg->msg_name);

		if (msg->msg_namelen < sizeof(*addr)) {
			release_sock(sk);
			return -EINVAL;
		}

		release_sock(sk);

		return nfc_llcp_send_ui_frame(llcp_sock, addr->dsap, addr->ssap,
					      msg, len);
	}

	if (sk->sk_state != LLCP_CONNECTED) {
		release_sock(sk);
		return -ENOTCONN;
	}

	release_sock(sk);

	return nfc_llcp_send_i_frame(llcp_sock, msg, len);
}