void bgp_send_delayed_eor(struct bgp *bgp)
{
	struct peer *peer;
	struct listnode *node, *nnode;

	/* EOR message sent in bgp_write_proceed_actions */
	for (ALL_LIST_ELEMENTS(bgp->peer, node, nnode, peer))
		bgp_write_proceed_actions(peer);
}