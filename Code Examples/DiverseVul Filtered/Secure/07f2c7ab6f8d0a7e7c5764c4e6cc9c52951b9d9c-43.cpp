struct sctp_chunk *sctp_chunkify(struct sk_buff *skb,
				 const struct sctp_association *asoc,
				 struct sock *sk, gfp_t gfp)
{
	struct sctp_chunk *retval;

	retval = kmem_cache_zalloc(sctp_chunk_cachep, gfp);

	if (!retval)
		goto nodata;
	if (!sk)
		pr_debug("%s: chunkifying skb:%p w/o an sk\n", __func__, skb);

	INIT_LIST_HEAD(&retval->list);
	retval->skb		= skb;
	retval->asoc		= (struct sctp_association *)asoc;
	retval->singleton	= 1;

	retval->fast_retransmit = SCTP_CAN_FRTX;

	/* Polish the bead hole.  */
	INIT_LIST_HEAD(&retval->transmitted_list);
	INIT_LIST_HEAD(&retval->frag_list);
	SCTP_DBG_OBJCNT_INC(chunk);
	refcount_set(&retval->refcnt, 1);

nodata:
	return retval;
}