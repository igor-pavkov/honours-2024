static inline int ndisc_is_useropt(struct nd_opt_hdr *opt)
{
	return opt->nd_opt_type == ND_OPT_RDNSS ||
		opt->nd_opt_type == ND_OPT_DNSSL;
}