static inline int sco_send_frame(struct sock *sk, struct msghdr *msg, int len)
{
	struct sco_conn *conn = sco_pi(sk)->conn;
	struct sk_buff *skb;
	int err, count;

	/* Check outgoing MTU */
	if (len > conn->mtu)
		return -EINVAL;

	BT_DBG("sk %p len %d", sk, len);

	count = min_t(unsigned int, conn->mtu, len);
	skb = bt_skb_send_alloc(sk, count,
			msg->msg_flags & MSG_DONTWAIT, &err);
	if (!skb)
		return err;

	if (memcpy_fromiovec(skb_put(skb, count), msg->msg_iov, count)) {
		kfree_skb(skb);
		return -EFAULT;
	}

	hci_send_sco(conn->hcon, skb);

	return count;
}