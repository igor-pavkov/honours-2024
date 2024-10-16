SCTP_STATIC int sctp_init_sock(struct sock *sk)
{
	struct sctp_endpoint *ep;
	struct sctp_sock *sp;

	SCTP_DEBUG_PRINTK("sctp_init_sock(sk: %p)\n", sk);

	sp = sctp_sk(sk);

	/* Initialize the SCTP per socket area.  */
	switch (sk->sk_type) {
	case SOCK_SEQPACKET:
		sp->type = SCTP_SOCKET_UDP;
		break;
	case SOCK_STREAM:
		sp->type = SCTP_SOCKET_TCP;
		break;
	default:
		return -ESOCKTNOSUPPORT;
	}

	/* Initialize default send parameters. These parameters can be
	 * modified with the SCTP_DEFAULT_SEND_PARAM socket option.
	 */
	sp->default_stream = 0;
	sp->default_ppid = 0;
	sp->default_flags = 0;
	sp->default_context = 0;
	sp->default_timetolive = 0;

	sp->default_rcv_context = 0;

	/* Initialize default setup parameters. These parameters
	 * can be modified with the SCTP_INITMSG socket option or
	 * overridden by the SCTP_INIT CMSG.
	 */
	sp->initmsg.sinit_num_ostreams   = sctp_max_outstreams;
	sp->initmsg.sinit_max_instreams  = sctp_max_instreams;
	sp->initmsg.sinit_max_attempts   = sctp_max_retrans_init;
	sp->initmsg.sinit_max_init_timeo = sctp_rto_max;

	/* Initialize default RTO related parameters.  These parameters can
	 * be modified for with the SCTP_RTOINFO socket option.
	 */
	sp->rtoinfo.srto_initial = sctp_rto_initial;
	sp->rtoinfo.srto_max     = sctp_rto_max;
	sp->rtoinfo.srto_min     = sctp_rto_min;

	/* Initialize default association related parameters. These parameters
	 * can be modified with the SCTP_ASSOCINFO socket option.
	 */
	sp->assocparams.sasoc_asocmaxrxt = sctp_max_retrans_association;
	sp->assocparams.sasoc_number_peer_destinations = 0;
	sp->assocparams.sasoc_peer_rwnd = 0;
	sp->assocparams.sasoc_local_rwnd = 0;
	sp->assocparams.sasoc_cookie_life = sctp_valid_cookie_life;

	/* Initialize default event subscriptions. By default, all the
	 * options are off.
	 */
	memset(&sp->subscribe, 0, sizeof(struct sctp_event_subscribe));

	/* Default Peer Address Parameters.  These defaults can
	 * be modified via SCTP_PEER_ADDR_PARAMS
	 */
	sp->hbinterval  = sctp_hb_interval;
	sp->pathmaxrxt  = sctp_max_retrans_path;
	sp->pathmtu     = 0; // allow default discovery
	sp->sackdelay   = sctp_sack_timeout;
	sp->param_flags = SPP_HB_ENABLE |
			  SPP_PMTUD_ENABLE |
			  SPP_SACKDELAY_ENABLE;

	/* If enabled no SCTP message fragmentation will be performed.
	 * Configure through SCTP_DISABLE_FRAGMENTS socket option.
	 */
	sp->disable_fragments = 0;

	/* Enable Nagle algorithm by default.  */
	sp->nodelay           = 0;

	/* Enable by default. */
	sp->v4mapped          = 1;

	/* Auto-close idle associations after the configured
	 * number of seconds.  A value of 0 disables this
	 * feature.  Configure through the SCTP_AUTOCLOSE socket option,
	 * for UDP-style sockets only.
	 */
	sp->autoclose         = 0;

	/* User specified fragmentation limit. */
	sp->user_frag         = 0;

	sp->adaptation_ind = 0;

	sp->pf = sctp_get_pf_specific(sk->sk_family);

	/* Control variables for partial data delivery. */
	sp->pd_mode           = 0;
	skb_queue_head_init(&sp->pd_lobby);

	/* Create a per socket endpoint structure.  Even if we
	 * change the data structure relationships, this may still
	 * be useful for storing pre-connect address information.
	 */
	ep = sctp_endpoint_new(sk, GFP_KERNEL);
	if (!ep)
		return -ENOMEM;

	sp->ep = ep;
	sp->hmac = NULL;

	SCTP_DBG_OBJCNT_INC(sock);
	return 0;
}