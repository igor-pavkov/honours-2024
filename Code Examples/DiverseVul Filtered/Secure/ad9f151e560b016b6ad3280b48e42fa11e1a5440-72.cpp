__be64 nf_jiffies64_to_msecs(u64 input)
{
	return cpu_to_be64(jiffies64_to_msecs(input));
}