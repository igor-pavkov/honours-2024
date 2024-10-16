void tcp_time_wait(struct sock *sk, int state, int timeo)
{
	const struct inet_connection_sock *icsk = inet_csk(sk);
	const struct tcp_sock *tp = tcp_sk(sk);
	struct inet_timewait_sock *tw;
	struct inet_timewait_death_row *tcp_death_row = &sock_net(sk)->ipv4.tcp_death_row;

	tw = inet_twsk_alloc(sk, tcp_death_row, state);

	if (tw) {
		struct tcp_timewait_sock *tcptw = tcp_twsk((struct sock *)tw);
		const int rto = (icsk->icsk_rto << 2) - (icsk->icsk_rto >> 1);
		struct inet_sock *inet = inet_sk(sk);

		tw->tw_transparent	= inet->transparent;
		tw->tw_rcv_wscale	= tp->rx_opt.rcv_wscale;
		tcptw->tw_rcv_nxt	= tp->rcv_nxt;
		tcptw->tw_snd_nxt	= tp->snd_nxt;
		tcptw->tw_rcv_wnd	= tcp_receive_window(tp);
		tcptw->tw_ts_recent	= tp->rx_opt.ts_recent;
		tcptw->tw_ts_recent_stamp = tp->rx_opt.ts_recent_stamp;
		tcptw->tw_ts_offset	= tp->tsoffset;
		tcptw->tw_last_oow_ack_time = 0;

#if IS_ENABLED(CONFIG_IPV6)
		if (tw->tw_family == PF_INET6) {
			struct ipv6_pinfo *np = inet6_sk(sk);

			tw->tw_v6_daddr = sk->sk_v6_daddr;
			tw->tw_v6_rcv_saddr = sk->sk_v6_rcv_saddr;
			tw->tw_tclass = np->tclass;
			tw->tw_flowlabel = be32_to_cpu(np->flow_label & IPV6_FLOWLABEL_MASK);
			tw->tw_ipv6only = sk->sk_ipv6only;
		}
#endif

#ifdef CONFIG_TCP_MD5SIG
		/*
		 * The timewait bucket does not have the key DB from the
		 * sock structure. We just make a quick copy of the
		 * md5 key being used (if indeed we are using one)
		 * so the timewait ack generating code has the key.
		 */
		do {
			struct tcp_md5sig_key *key;
			tcptw->tw_md5_key = NULL;
			key = tp->af_specific->md5_lookup(sk, sk);
			if (key) {
				tcptw->tw_md5_key = kmemdup(key, sizeof(*key), GFP_ATOMIC);
				if (tcptw->tw_md5_key && !tcp_alloc_md5sig_pool())
					BUG();
			}
		} while (0);
#endif

		/* Get the TIME_WAIT timeout firing. */
		if (timeo < rto)
			timeo = rto;

		tw->tw_timeout = TCP_TIMEWAIT_LEN;
		if (state == TCP_TIME_WAIT)
			timeo = TCP_TIMEWAIT_LEN;

		inet_twsk_schedule(tw, timeo);
		/* Linkage updates. */
		__inet_twsk_hashdance(tw, sk, &tcp_hashinfo);
		inet_twsk_put(tw);
	} else {
		/* Sorry, if we're out of memory, just CLOSE this
		 * socket up.  We've got bigger problems than
		 * non-graceful socket closings.
		 */
		NET_INC_STATS(sock_net(sk), LINUX_MIB_TCPTIMEWAITOVERFLOW);
	}

	tcp_update_metrics(sk);
	tcp_done(sk);
}