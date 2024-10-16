static int udp_gro_complete_segment(struct sk_buff *skb)
{
	struct udphdr *uh = udp_hdr(skb);

	skb->csum_start = (unsigned char *)uh - skb->head;
	skb->csum_offset = offsetof(struct udphdr, check);
	skb->ip_summed = CHECKSUM_PARTIAL;

	skb_shinfo(skb)->gso_segs = NAPI_GRO_CB(skb)->count;
	skb_shinfo(skb)->gso_type |= SKB_GSO_UDP_L4;
	return 0;
}