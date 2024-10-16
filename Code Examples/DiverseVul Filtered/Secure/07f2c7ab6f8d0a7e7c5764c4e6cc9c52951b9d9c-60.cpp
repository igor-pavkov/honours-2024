int sctp_chunk_iif(const struct sctp_chunk *chunk)
{
	struct sk_buff *skb = chunk->skb;

	return SCTP_INPUT_CB(skb)->af->skb_iif(skb);
}