static int raw_enable_errfilter(struct net_device *dev, struct sock *sk,
				can_err_mask_t err_mask)
{
	int err = 0;

	if (err_mask)
		err = can_rx_register(dev, 0, err_mask | CAN_ERR_FLAG,
				      raw_rcv, sk, "raw");

	return err;
}