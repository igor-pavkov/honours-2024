static int hidp_verify_sockets(struct socket *ctrl_sock,
			       struct socket *intr_sock)
{
	struct l2cap_chan *ctrl_chan, *intr_chan;
	struct bt_sock *ctrl, *intr;
	struct hidp_session *session;

	if (!l2cap_is_socket(ctrl_sock) || !l2cap_is_socket(intr_sock))
		return -EINVAL;

	ctrl_chan = l2cap_pi(ctrl_sock->sk)->chan;
	intr_chan = l2cap_pi(intr_sock->sk)->chan;

	if (bacmp(&ctrl_chan->src, &intr_chan->src) ||
	    bacmp(&ctrl_chan->dst, &intr_chan->dst))
		return -ENOTUNIQ;

	ctrl = bt_sk(ctrl_sock->sk);
	intr = bt_sk(intr_sock->sk);

	if (ctrl->sk.sk_state != BT_CONNECTED ||
	    intr->sk.sk_state != BT_CONNECTED)
		return -EBADFD;

	/* early session check, we check again during session registration */
	session = hidp_session_find(&ctrl_chan->dst);
	if (session) {
		hidp_session_put(session);
		return -EEXIST;
	}

	return 0;
}