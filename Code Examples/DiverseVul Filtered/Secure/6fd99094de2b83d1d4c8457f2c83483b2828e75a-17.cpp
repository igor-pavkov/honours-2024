static u32 ndisc_hash(const void *pkey,
		      const struct net_device *dev,
		      __u32 *hash_rnd)
{
	return ndisc_hashfn(pkey, dev, hash_rnd);
}