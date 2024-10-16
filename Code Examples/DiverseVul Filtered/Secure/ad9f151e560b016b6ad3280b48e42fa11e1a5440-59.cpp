int nf_msecs_to_jiffies64(const struct nlattr *nla, u64 *result)
{
	u64 ms = be64_to_cpu(nla_get_be64(nla));
	u64 max = (u64)(~((u64)0));

	max = div_u64(max, NSEC_PER_MSEC);
	if (ms >= max)
		return -ERANGE;

	ms *= NSEC_PER_MSEC;
	*result = nsecs_to_jiffies64(ms);
	return 0;
}