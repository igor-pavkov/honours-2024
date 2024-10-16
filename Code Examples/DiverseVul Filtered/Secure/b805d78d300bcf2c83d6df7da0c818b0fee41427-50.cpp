static inline unsigned int xfrm_spdinfo_msgsize(void)
{
	return NLMSG_ALIGN(4)
	       + nla_total_size(sizeof(struct xfrmu_spdinfo))
	       + nla_total_size(sizeof(struct xfrmu_spdhinfo))
	       + nla_total_size(sizeof(struct xfrmu_spdhthresh))
	       + nla_total_size(sizeof(struct xfrmu_spdhthresh));
}