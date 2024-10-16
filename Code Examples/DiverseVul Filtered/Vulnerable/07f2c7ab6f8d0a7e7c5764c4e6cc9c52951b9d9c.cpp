static struct sctp_chunk *_sctp_make_chunk(const struct sctp_association *asoc,
					   __u8 type, __u8 flags, int paylen,
					   gfp_t gfp)
{
	struct sctp_chunkhdr *chunk_hdr;
	struct sctp_chunk *retval;
	struct sk_buff *skb;
	struct sock *sk;

	/* No need to allocate LL here, as this is only a chunk. */
	skb = alloc_skb(SCTP_PAD4(sizeof(*chunk_hdr) + paylen), gfp);
	if (!skb)
		goto nodata;

	/* Make room for the chunk header.  */
	chunk_hdr = (struct sctp_chunkhdr *)skb_put(skb, sizeof(*chunk_hdr));
	chunk_hdr->type	  = type;
	chunk_hdr->flags  = flags;
	chunk_hdr->length = htons(sizeof(*chunk_hdr));

	sk = asoc ? asoc->base.sk : NULL;
	retval = sctp_chunkify(skb, asoc, sk, gfp);
	if (!retval) {
		kfree_skb(skb);
		goto nodata;
	}

	retval->chunk_hdr = chunk_hdr;
	retval->chunk_end = ((__u8 *)chunk_hdr) + sizeof(*chunk_hdr);

	/* Determine if the chunk needs to be authenticated */
	if (sctp_auth_send_cid(type, asoc))
		retval->auth = 1;

	return retval;
nodata:
	return NULL;
}