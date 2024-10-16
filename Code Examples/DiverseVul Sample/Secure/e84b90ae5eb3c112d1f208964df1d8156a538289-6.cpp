static inline void raw_disable_allfilters(struct net_device *dev,
					  struct sock *sk)
{
	struct raw_sock *ro = raw_sk(sk);

	raw_disable_filters(dev, sk, ro->filter, ro->count);
	raw_disable_errfilter(dev, sk, ro->err_mask);
}