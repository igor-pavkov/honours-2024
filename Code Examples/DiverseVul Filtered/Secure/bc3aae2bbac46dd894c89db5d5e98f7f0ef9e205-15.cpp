static unsigned int ipv4_default_advmss(const struct dst_entry *dst)
{
	unsigned int header_size = sizeof(struct tcphdr) + sizeof(struct iphdr);
	unsigned int advmss = max_t(unsigned int, dst->dev->mtu - header_size,
				    ip_rt_min_advmss);

	return min(advmss, IPV4_MAX_PMTU - header_size);
}