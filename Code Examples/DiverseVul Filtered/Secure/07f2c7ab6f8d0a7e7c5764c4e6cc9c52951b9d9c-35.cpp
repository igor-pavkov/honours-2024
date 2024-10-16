struct sctp_association *sctp_make_temp_asoc(const struct sctp_endpoint *ep,
					     struct sctp_chunk *chunk,
					     gfp_t gfp)
{
	struct sctp_association *asoc;
	enum sctp_scope scope;
	struct sk_buff *skb;

	/* Create the bare association.  */
	scope = sctp_scope(sctp_source(chunk));
	asoc = sctp_association_new(ep, ep->base.sk, scope, gfp);
	if (!asoc)
		goto nodata;
	asoc->temp = 1;
	skb = chunk->skb;
	/* Create an entry for the source address of the packet.  */
	SCTP_INPUT_CB(skb)->af->from_skb(&asoc->c.peer_addr, skb, 1);

nodata:
	return asoc;
}