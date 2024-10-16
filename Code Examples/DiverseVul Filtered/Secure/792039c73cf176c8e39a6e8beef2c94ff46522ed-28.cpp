static void l2cap_sock_state_change_cb(struct l2cap_chan *chan, int state)
{
	struct sock *sk = chan->data;

	sk->sk_state = state;
}