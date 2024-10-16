void PCPPublicAddressChanged(int * sockets, int n_sockets)
#endif
{
	/* according to RFC 6887  8.5 :
	 *   if the external IP address(es) of the NAT (controlled by
	 *   the PCP server) changes, the Epoch time MUST be reset. */
	epoch_origin = upnp_time();
#ifdef ENABLE_IPV6
	PCPSendUnsolicitedAnnounce(sockets, n_sockets, socket6);
#else /* IPv4 Only */
	PCPSendUnsolicitedAnnounce(sockets, n_sockets);
#endif
}