int rfcomm_dev_ioctl(struct sock *sk, unsigned int cmd, void __user *arg)
{
	BT_DBG("cmd %d arg %p", cmd, arg);

	switch (cmd) {
	case RFCOMMCREATEDEV:
		return rfcomm_create_dev(sk, arg);

	case RFCOMMRELEASEDEV:
		return rfcomm_release_dev(arg);

	case RFCOMMGETDEVLIST:
		return rfcomm_get_dev_list(arg);

	case RFCOMMGETDEVINFO:
		return rfcomm_get_dev_info(arg);
	}

	return -EINVAL;
}