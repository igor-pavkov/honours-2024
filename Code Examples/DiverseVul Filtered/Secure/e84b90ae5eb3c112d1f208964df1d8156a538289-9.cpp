static int raw_enable_allfilters(struct net_device *dev, struct sock *sk)
{
	struct raw_sock *ro = raw_sk(sk);
	int err;

	err = raw_enable_filters(dev, sk, ro->filter, ro->count);
	if (!err) {
		err = raw_enable_errfilter(dev, sk, ro->err_mask);
		if (err)
			raw_disable_filters(dev, sk, ro->filter, ro->count);
	}

	return err;
}