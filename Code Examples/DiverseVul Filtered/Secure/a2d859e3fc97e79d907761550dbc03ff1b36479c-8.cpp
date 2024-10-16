static void sctp_control_release_owner(struct sk_buff *skb)
{
	struct sctp_chunk *chunk = skb_shinfo(skb)->destructor_arg;

	if (chunk->shkey) {
		struct sctp_shared_key *shkey = chunk->shkey;
		struct sctp_association *asoc = chunk->asoc;

		/* refcnt == 2 and !list_empty mean after this release, it's
		 * not being used anywhere, and it's time to notify userland
		 * that this shkey can be freed if it's been deactivated.
		 */
		if (shkey->deactivated && !list_empty(&shkey->key_list) &&
		    refcount_read(&shkey->refcnt) == 2) {
			struct sctp_ulpevent *ev;

			ev = sctp_ulpevent_make_authkey(asoc, shkey->key_id,
							SCTP_AUTH_FREE_KEY,
							GFP_KERNEL);
			if (ev)
				asoc->stream.si->enqueue_event(&asoc->ulpq, ev);
		}
		sctp_auth_shkey_release(chunk->shkey);
	}
}