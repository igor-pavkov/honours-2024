static void *sctp_addto_param(struct sctp_chunk *chunk, int len,
			      const void *data)
{
	int chunklen = ntohs(chunk->chunk_hdr->length);
	void *target;

	target = skb_put(chunk->skb, len);

	if (data)
		memcpy(target, data, len);
	else
		memset(target, 0, len);

	/* Adjust the chunk length field.  */
	chunk->chunk_hdr->length = htons(chunklen + len);
	chunk->chunk_end = skb_tail_pointer(chunk->skb);

	return target;
}