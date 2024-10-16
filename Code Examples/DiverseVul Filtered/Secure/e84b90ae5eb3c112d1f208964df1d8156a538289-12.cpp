static void raw_disable_filters(struct net_device *dev, struct sock *sk,
			      struct can_filter *filter, int count)
{
	int i;

	for (i = 0; i < count; i++)
		can_rx_unregister(dev, filter[i].can_id, filter[i].can_mask,
				  raw_rcv, sk);
}