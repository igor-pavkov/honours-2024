struct sctp_chunk *sctp_make_pad(const struct sctp_association *asoc, int len)
{
	struct sctp_chunk *retval;

	retval = sctp_make_control(asoc, SCTP_CID_PAD, 0, len, GFP_ATOMIC);
	if (!retval)
		return NULL;

	skb_put_zero(retval->skb, len);
	retval->chunk_hdr->length = htons(ntohs(retval->chunk_hdr->length) + len);
	retval->chunk_end = skb_tail_pointer(retval->skb);

	return retval;
}