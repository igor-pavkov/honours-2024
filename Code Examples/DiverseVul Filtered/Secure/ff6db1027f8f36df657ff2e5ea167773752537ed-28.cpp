static int bgp_collision_detect(struct peer *new, struct in_addr remote_id)
{
	struct peer *peer;

	/*
	 * Upon receipt of an OPEN message, the local system must examine
	 * all of its connections that are in the OpenConfirm state.  A BGP
	 * speaker may also examine connections in an OpenSent state if it
	 * knows the BGP Identifier of the peer by means outside of the
	 * protocol.  If among these connections there is a connection to a
	 * remote BGP speaker whose BGP Identifier equals the one in the
	 * OPEN message, then the local system performs the following
	 * collision resolution procedure:
	 */
	peer = new->doppelganger;
	if (peer == NULL)
		return 0;

	/*
	 * Do not accept the new connection in Established or Clearing
	 * states. Note that a peer GR is handled by closing the existing
	 * connection upon receipt of new one.
	 */
	if (peer_established(peer) || peer->status == Clearing) {
		bgp_notify_send(new, BGP_NOTIFY_CEASE,
				BGP_NOTIFY_CEASE_COLLISION_RESOLUTION);
		return -1;
	}

	if ((peer->status != OpenConfirm) && (peer->status != OpenSent))
		return 0;

	/*
	 * 1. The BGP Identifier of the local system is
	 * compared to the BGP Identifier of the remote
	 * system (as specified in the OPEN message).
	 *
	 * If the BGP Identifiers of the peers
	 * involved in the connection collision
	 * are identical, then the connection
	 * initiated by the BGP speaker with the
	 * larger AS number is preserved.
	 */
	if (ntohl(peer->local_id.s_addr) < ntohl(remote_id.s_addr)
	    || (ntohl(peer->local_id.s_addr) == ntohl(remote_id.s_addr)
		&& peer->local_as < peer->as))
		if (!CHECK_FLAG(peer->sflags, PEER_STATUS_ACCEPT_PEER)) {
			/*
			 * 2. If the value of the local BGP
			 * Identifier is less than the remote one,
			 * the local system closes BGP connection
			 * that already exists (the one that is
			 * already in the OpenConfirm state),
			 * and accepts BGP connection initiated by
			 * the remote system.
			 */
			bgp_notify_send(peer, BGP_NOTIFY_CEASE,
					BGP_NOTIFY_CEASE_COLLISION_RESOLUTION);
			return 1;
		} else {
			bgp_notify_send(new, BGP_NOTIFY_CEASE,
					BGP_NOTIFY_CEASE_COLLISION_RESOLUTION);
			return -1;
		}
	else {
		if (ntohl(peer->local_id.s_addr) == ntohl(remote_id.s_addr)
		    && peer->local_as == peer->as)
			flog_err(EC_BGP_ROUTER_ID_SAME,
				 "Peer's router-id %pI4 is the same as ours",
				 &remote_id);

		/*
		 * 3. Otherwise, the local system closes newly
		 * created BGP connection (the one associated with the
		 * newly received OPEN message), and continues to use
		 * the existing one (the one that is already in the
		 * OpenConfirm state).
		 */
		if (CHECK_FLAG(peer->sflags, PEER_STATUS_ACCEPT_PEER)) {
			bgp_notify_send(peer, BGP_NOTIFY_CEASE,
					BGP_NOTIFY_CEASE_COLLISION_RESOLUTION);
			return 1;
		} else {
			bgp_notify_send(new, BGP_NOTIFY_CEASE,
					BGP_NOTIFY_CEASE_COLLISION_RESOLUTION);
			return -1;
		}
	}
}