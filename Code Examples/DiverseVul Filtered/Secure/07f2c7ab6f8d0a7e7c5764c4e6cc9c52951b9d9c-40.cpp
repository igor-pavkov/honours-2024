void *sctp_addto_chunk(struct sctp_chunk *chunk, int len, const void *data)
{
	int chunklen = ntohs(chunk->chunk_hdr->length);
	int padlen = SCTP_PAD4(chunklen) - chunklen;
	void *target;

	skb_put_zero(chunk->skb, padlen);
	target = skb_put_data(chunk->skb, data, len);

	/* Adjust the chunk length field.  */
	chunk->chunk_hdr->length = htons(chunklen + padlen + len);
	chunk->chunk_end = skb_tail_pointer(chunk->skb);

	return target;
}