static __be16 atalk_checksum(const struct sk_buff *skb, int len)
{
	unsigned long sum;

	/* skip header 4 bytes */
	sum = atalk_sum_skb(skb, 4, len-4, 0);

	/* Use 0xFFFF for 0. 0 itself means none */
	return sum ? htons((unsigned short)sum) : htons(0xFFFF);
}