report_event(
	int	err,		/* error code */
	struct peer *peer,	/* peer structure pointer */
	const char *str		/* protostats string */
	)
{
	char	statstr[NTP_MAXSTRLEN];
	int	i;
	size_t	len;

	/*
	 * Report the error to the protostats file, system log and
	 * trappers.
	 */
	if (peer == NULL) {

		/*
		 * Discard a system report if the number of reports of
		 * the same type exceeds the maximum.
		 */
		if (ctl_sys_last_event != (u_char)err)
			ctl_sys_num_events= 0;
		if (ctl_sys_num_events >= CTL_SYS_MAXEVENTS)
			return;

		ctl_sys_last_event = (u_char)err;
		ctl_sys_num_events++;
		snprintf(statstr, sizeof(statstr),
		    "0.0.0.0 %04x %02x %s",
		    ctlsysstatus(), err, eventstr(err));
		if (str != NULL) {
			len = strlen(statstr);
			snprintf(statstr + len, sizeof(statstr) - len,
			    " %s", str);
		}
		NLOG(NLOG_SYSEVENT)
			msyslog(LOG_INFO, "%s", statstr);
	} else {

		/*
		 * Discard a peer report if the number of reports of
		 * the same type exceeds the maximum for that peer.
		 */
		const char *	src;
		u_char		errlast;

		errlast = (u_char)err & ~PEER_EVENT;
		if (peer->last_event == errlast)
			peer->num_events = 0;
		if (peer->num_events >= CTL_PEER_MAXEVENTS)
			return;

		peer->last_event = errlast;
		peer->num_events++;
		if (ISREFCLOCKADR(&peer->srcadr))
			src = refnumtoa(&peer->srcadr);
		else
			src = stoa(&peer->srcadr);

		snprintf(statstr, sizeof(statstr),
		    "%s %04x %02x %s", src,
		    ctlpeerstatus(peer), err, eventstr(err));
		if (str != NULL) {
			len = strlen(statstr);
			snprintf(statstr + len, sizeof(statstr) - len,
			    " %s", str);
		}
		NLOG(NLOG_PEEREVENT)
			msyslog(LOG_INFO, "%s", statstr);
	}
	record_proto_stats(statstr);
#if DEBUG
	if (debug)
		printf("event at %lu %s\n", current_time, statstr);
#endif

	/*
	 * If no trappers, return.
	 */
	if (num_ctl_traps <= 0)
		return;

	/*
	 * Set up the outgoing packet variables
	 */
	res_opcode = CTL_OP_ASYNCMSG;
	res_offset = 0;
	res_async = TRUE;
	res_authenticate = FALSE;
	datapt = rpkt.u.data;
	dataend = &rpkt.u.data[CTL_MAX_DATA_LEN];
	if (!(err & PEER_EVENT)) {
		rpkt.associd = 0;
		rpkt.status = htons(ctlsysstatus());

		/* Include the core system variables and the list. */
		for (i = 1; i <= CS_VARLIST; i++)
			ctl_putsys(i);
	} else {
		INSIST(peer != NULL);
		rpkt.associd = htons(peer->associd);
		rpkt.status = htons(ctlpeerstatus(peer));

		/* Dump it all. Later, maybe less. */
		for (i = 1; i <= CP_MAX_NOAUTOKEY; i++)
			ctl_putpeer(i, peer);
#ifdef REFCLOCK
		/*
		 * for clock exception events: add clock variables to
		 * reflect info on exception
		 */
		if (err == PEVNT_CLOCK) {
			struct refclockstat cs;
			struct ctl_var *kv;

			cs.kv_list = NULL;
			refclock_control(&peer->srcadr, NULL, &cs);

			ctl_puthex("refclockstatus",
				   ctlclkstatus(&cs));

			for (i = 1; i <= CC_MAXCODE; i++)
				ctl_putclock(i, &cs, FALSE);
			for (kv = cs.kv_list;
			     kv != NULL && !(EOV & kv->flags);
			     kv++)
				if (DEF & kv->flags)
					ctl_putdata(kv->text,
						    strlen(kv->text),
						    FALSE);
			free_varlist(cs.kv_list);
		}
#endif /* REFCLOCK */
	}

	/*
	 * We're done, return.
	 */
	ctl_flushpkt(0);
}