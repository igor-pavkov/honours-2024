static void read_mru_list(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	const char		nonce_text[] =		"nonce";
	const char		frags_text[] =		"frags";
	const char		limit_text[] =		"limit";
	const char		mincount_text[] =	"mincount";
	const char		resall_text[] =		"resall";
	const char		resany_text[] =		"resany";
	const char		maxlstint_text[] =	"maxlstint";
	const char		laddr_text[] =		"laddr";
	const char		resaxx_fmt[] =		"0x%hx";
	u_int			limit;
	u_short			frags;
	u_short			resall;
	u_short			resany;
	int			mincount;
	u_int			maxlstint;
	sockaddr_u		laddr;
	struct interface *	lcladr;
	u_int			count;
	u_int			ui;
	u_int			uf;
	l_fp			last[16];
	sockaddr_u		addr[COUNTOF(last)];
	char			buf[128];
	struct ctl_var *	in_parms;
	const struct ctl_var *	v;
	char *			val;
	const char *		pch;
	char *			pnonce;
	int			nonce_valid;
	size_t			i;
	int			priors;
	u_short			hash;
	mon_entry *		mon;
	mon_entry *		prior_mon;
	l_fp			now;

	if (RES_NOMRULIST & restrict_mask) {
		ctl_error(CERR_PERMISSION);
		NLOG(NLOG_SYSINFO)
			msyslog(LOG_NOTICE,
				"mrulist from %s rejected due to nomrulist restriction",
				stoa(&rbufp->recv_srcadr));
		sys_restricted++;
		return;
	}
	/*
	 * fill in_parms var list with all possible input parameters.
	 */
	in_parms = NULL;
	set_var(&in_parms, nonce_text, sizeof(nonce_text), 0);
	set_var(&in_parms, frags_text, sizeof(frags_text), 0);
	set_var(&in_parms, limit_text, sizeof(limit_text), 0);
	set_var(&in_parms, mincount_text, sizeof(mincount_text), 0);
	set_var(&in_parms, resall_text, sizeof(resall_text), 0);
	set_var(&in_parms, resany_text, sizeof(resany_text), 0);
	set_var(&in_parms, maxlstint_text, sizeof(maxlstint_text), 0);
	set_var(&in_parms, laddr_text, sizeof(laddr_text), 0);
	for (i = 0; i < COUNTOF(last); i++) {
		snprintf(buf, sizeof(buf), last_fmt, (int)i);
		set_var(&in_parms, buf, strlen(buf) + 1, 0);
		snprintf(buf, sizeof(buf), addr_fmt, (int)i);
		set_var(&in_parms, buf, strlen(buf) + 1, 0);
	}

	/* decode input parms */
	pnonce = NULL;
	frags = 0;
	limit = 0;
	mincount = 0;
	resall = 0;
	resany = 0;
	maxlstint = 0;
	lcladr = NULL;
	priors = 0;
	ZERO(last);
	ZERO(addr);

	while (NULL != (v = ctl_getitem(in_parms, &val)) &&
	       !(EOV & v->flags)) {
		int si;

		if (!strcmp(nonce_text, v->text)) {
			if (NULL != pnonce)
				free(pnonce);
			pnonce = estrdup(val);
		} else if (!strcmp(frags_text, v->text)) {
			sscanf(val, "%hu", &frags);
		} else if (!strcmp(limit_text, v->text)) {
			sscanf(val, "%u", &limit);
		} else if (!strcmp(mincount_text, v->text)) {
			if (1 != sscanf(val, "%d", &mincount) ||
			    mincount < 0)
				mincount = 0;
		} else if (!strcmp(resall_text, v->text)) {
			sscanf(val, resaxx_fmt, &resall);
		} else if (!strcmp(resany_text, v->text)) {
			sscanf(val, resaxx_fmt, &resany);
		} else if (!strcmp(maxlstint_text, v->text)) {
			sscanf(val, "%u", &maxlstint);
		} else if (!strcmp(laddr_text, v->text)) {
			if (decodenetnum(val, &laddr))
				lcladr = getinterface(&laddr, 0);
		} else if (1 == sscanf(v->text, last_fmt, &si) &&
			   (size_t)si < COUNTOF(last)) {
			if (2 == sscanf(val, "0x%08x.%08x", &ui, &uf)) {
				last[si].l_ui = ui;
				last[si].l_uf = uf;
				if (!SOCK_UNSPEC(&addr[si]) &&
				    si == priors)
					priors++;
			}
		} else if (1 == sscanf(v->text, addr_fmt, &si) &&
			   (size_t)si < COUNTOF(addr)) {
			if (decodenetnum(val, &addr[si])
			    && last[si].l_ui && last[si].l_uf &&
			    si == priors)
				priors++;
		}
	}
	free_varlist(in_parms);
	in_parms = NULL;

	/* return no responses until the nonce is validated */
	if (NULL == pnonce)
		return;

	nonce_valid = validate_nonce(pnonce, rbufp);
	free(pnonce);
	if (!nonce_valid)
		return;

	if ((0 == frags && !(0 < limit && limit <= MRU_ROW_LIMIT)) ||
	    frags > MRU_FRAGS_LIMIT) {
		ctl_error(CERR_BADVALUE);
		return;
	}

	/*
	 * If either frags or limit is not given, use the max.
	 */
	if (0 != frags && 0 == limit)
		limit = UINT_MAX;
	else if (0 != limit && 0 == frags)
		frags = MRU_FRAGS_LIMIT;

	/*
	 * Find the starting point if one was provided.
	 */
	mon = NULL;
	for (i = 0; i < (size_t)priors; i++) {
		hash = MON_HASH(&addr[i]);
		for (mon = mon_hash[hash];
		     mon != NULL;
		     mon = mon->hash_next)
			if (ADDR_PORT_EQ(&mon->rmtadr, &addr[i]))
				break;
		if (mon != NULL) {
			if (L_ISEQU(&mon->last, &last[i]))
				break;
			mon = NULL;
		}
	}

	/* If a starting point was provided... */
	if (priors) {
		/* and none could be found unmodified... */
		if (NULL == mon) {
			/* tell ntpq to try again with older entries */
			ctl_error(CERR_UNKNOWNVAR);
			return;
		}
		/* confirm the prior entry used as starting point */
		ctl_putts("last.older", &mon->last);
		pch = sptoa(&mon->rmtadr);
		ctl_putunqstr("addr.older", pch, strlen(pch));

		/*
		 * Move on to the first entry the client doesn't have,
		 * except in the special case of a limit of one.  In
		 * that case return the starting point entry.
		 */
		if (limit > 1)
			mon = PREV_DLIST(mon_mru_list, mon, mru);
	} else {	/* start with the oldest */
		mon = TAIL_DLIST(mon_mru_list, mru);
	}

	/*
	 * send up to limit= entries in up to frags= datagrams
	 */
	get_systime(&now);
	generate_nonce(rbufp, buf, sizeof(buf));
	ctl_putunqstr("nonce", buf, strlen(buf));
	prior_mon = NULL;
	for (count = 0;
	     mon != NULL && res_frags < frags && count < limit;
	     mon = PREV_DLIST(mon_mru_list, mon, mru)) {

		if (mon->count < mincount)
			continue;
		if (resall && resall != (resall & mon->flags))
			continue;
		if (resany && !(resany & mon->flags))
			continue;
		if (maxlstint > 0 && now.l_ui - mon->last.l_ui >
		    maxlstint)
			continue;
		if (lcladr != NULL && mon->lcladr != lcladr)
			continue;

		send_mru_entry(mon, count);
		if (!count)
			send_random_tag_value(0);
		count++;
		prior_mon = mon;
	}

	/*
	 * If this batch completes the MRU list, say so explicitly with
	 * a now= l_fp timestamp.
	 */
	if (NULL == mon) {
		if (count > 1)
			send_random_tag_value(count - 1);
		ctl_putts("now", &now);
		/* if any entries were returned confirm the last */
		if (prior_mon != NULL)
			ctl_putts("last.newest", &prior_mon->last);
	}
	ctl_flushpkt(0);
}