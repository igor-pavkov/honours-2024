static void sctp_control_set_owner_w(struct sctp_chunk *chunk)
{
	struct sctp_association *asoc = chunk->asoc;
	struct sk_buff *skb = chunk->skb;

	/* TODO: properly account for control chunks.
	 * To do it right we'll need:
	 *  1) endpoint if association isn't known.
	 *  2) proper memory accounting.
	 *
	 *  For now don't do anything for now.
	 */
	if (chunk->auth) {
		chunk->shkey = asoc->shkey;
		sctp_auth_shkey_hold(chunk->shkey);
	}
	skb->sk = asoc ? asoc->base.sk : NULL;
	skb_shinfo(skb)->destructor_arg = chunk;
	skb->destructor = sctp_control_release_owner;
}