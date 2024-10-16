static int raw_enable_filters(struct net_device *dev, struct sock *sk,
			      struct can_filter *filter, int count)
{
	int err = 0;
	int i;

	for (i = 0; i < count; i++) {
		err = can_rx_register(dev, filter[i].can_id,
				      filter[i].can_mask,
				      raw_rcv, sk, "raw");
		if (err) {
			/* clean up successfully registered filters */
			while (--i >= 0)
				can_rx_unregister(dev, filter[i].can_id,
						  filter[i].can_mask,
						  raw_rcv, sk);
			break;
		}
	}

	return err;
}