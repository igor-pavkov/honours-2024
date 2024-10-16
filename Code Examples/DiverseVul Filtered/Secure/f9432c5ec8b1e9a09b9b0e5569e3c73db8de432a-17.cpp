static int rfcomm_create_dev(struct sock *sk, void __user *arg)
{
	struct rfcomm_dev_req req;
	struct rfcomm_dlc *dlc;
	int id;

	if (copy_from_user(&req, arg, sizeof(req)))
		return -EFAULT;

	BT_DBG("sk %p dev_id %d flags 0x%x", sk, req.dev_id, req.flags);

	if (req.flags != NOCAP_FLAGS && !capable(CAP_NET_ADMIN))
		return -EPERM;

	if (req.flags & (1 << RFCOMM_REUSE_DLC)) {
		/* Socket must be connected */
		if (sk->sk_state != BT_CONNECTED)
			return -EBADFD;

		dlc = rfcomm_pi(sk)->dlc;
		rfcomm_dlc_hold(dlc);
	} else {
		dlc = rfcomm_dlc_alloc(GFP_KERNEL);
		if (!dlc)
			return -ENOMEM;
	}

	id = rfcomm_dev_add(&req, dlc);
	if (id < 0) {
		rfcomm_dlc_put(dlc);
		return id;
	}

	if (req.flags & (1 << RFCOMM_REUSE_DLC)) {
		/* DLC is now used by device.
		 * Socket must be disconnected */
		sk->sk_state = BT_CLOSED;
	}

	return id;
}