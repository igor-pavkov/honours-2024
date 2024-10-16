static inline void raw_disable_errfilter(struct net_device *dev,
					 struct sock *sk,
					 can_err_mask_t err_mask)

{
	if (err_mask)
		can_rx_unregister(dev, 0, err_mask | CAN_ERR_FLAG,
				  raw_rcv, sk);
}