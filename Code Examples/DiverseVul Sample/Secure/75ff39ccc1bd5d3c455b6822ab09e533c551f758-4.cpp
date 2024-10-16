static int tcp_clean_rtx_queue(struct sock *sk, int prior_fackets,
			       u32 prior_snd_una, int *acked,
			       struct tcp_sacktag_state *sack)
{
	const struct inet_connection_sock *icsk = inet_csk(sk);
	struct skb_mstamp first_ackt, last_ackt, now;
	struct tcp_sock *tp = tcp_sk(sk);
	u32 prior_sacked = tp->sacked_out;
	u32 reord = tp->packets_out;
	bool fully_acked = true;
	long sack_rtt_us = -1L;
	long seq_rtt_us = -1L;
	long ca_rtt_us = -1L;
	struct sk_buff *skb;
	u32 pkts_acked = 0;
	bool rtt_update;
	int flag = 0;

	first_ackt.v64 = 0;

	while ((skb = tcp_write_queue_head(sk)) && skb != tcp_send_head(sk)) {
		struct tcp_skb_cb *scb = TCP_SKB_CB(skb);
		u8 sacked = scb->sacked;
		u32 acked_pcount;

		tcp_ack_tstamp(sk, skb, prior_snd_una);

		/* Determine how many packets and what bytes were acked, tso and else */
		if (after(scb->end_seq, tp->snd_una)) {
			if (tcp_skb_pcount(skb) == 1 ||
			    !after(tp->snd_una, scb->seq))
				break;

			acked_pcount = tcp_tso_acked(sk, skb);
			if (!acked_pcount)
				break;

			fully_acked = false;
		} else {
			/* Speedup tcp_unlink_write_queue() and next loop */
			prefetchw(skb->next);
			acked_pcount = tcp_skb_pcount(skb);
		}

		if (unlikely(sacked & TCPCB_RETRANS)) {
			if (sacked & TCPCB_SACKED_RETRANS)
				tp->retrans_out -= acked_pcount;
			flag |= FLAG_RETRANS_DATA_ACKED;
		} else if (!(sacked & TCPCB_SACKED_ACKED)) {
			last_ackt = skb->skb_mstamp;
			WARN_ON_ONCE(last_ackt.v64 == 0);
			if (!first_ackt.v64)
				first_ackt = last_ackt;

			reord = min(pkts_acked, reord);
			if (!after(scb->end_seq, tp->high_seq))
				flag |= FLAG_ORIG_SACK_ACKED;
		}

		if (sacked & TCPCB_SACKED_ACKED) {
			tp->sacked_out -= acked_pcount;
		} else if (tcp_is_sack(tp)) {
			tp->delivered += acked_pcount;
			if (!tcp_skb_spurious_retrans(tp, skb))
				tcp_rack_advance(tp, &skb->skb_mstamp, sacked);
		}
		if (sacked & TCPCB_LOST)
			tp->lost_out -= acked_pcount;

		tp->packets_out -= acked_pcount;
		pkts_acked += acked_pcount;

		/* Initial outgoing SYN's get put onto the write_queue
		 * just like anything else we transmit.  It is not
		 * true data, and if we misinform our callers that
		 * this ACK acks real data, we will erroneously exit
		 * connection startup slow start one packet too
		 * quickly.  This is severely frowned upon behavior.
		 */
		if (likely(!(scb->tcp_flags & TCPHDR_SYN))) {
			flag |= FLAG_DATA_ACKED;
		} else {
			flag |= FLAG_SYN_ACKED;
			tp->retrans_stamp = 0;
		}

		if (!fully_acked)
			break;

		tcp_unlink_write_queue(skb, sk);
		sk_wmem_free_skb(sk, skb);
		if (unlikely(skb == tp->retransmit_skb_hint))
			tp->retransmit_skb_hint = NULL;
		if (unlikely(skb == tp->lost_skb_hint))
			tp->lost_skb_hint = NULL;
	}

	if (likely(between(tp->snd_up, prior_snd_una, tp->snd_una)))
		tp->snd_up = tp->snd_una;

	if (skb && (TCP_SKB_CB(skb)->sacked & TCPCB_SACKED_ACKED))
		flag |= FLAG_SACK_RENEGING;

	skb_mstamp_get(&now);
	if (likely(first_ackt.v64) && !(flag & FLAG_RETRANS_DATA_ACKED)) {
		seq_rtt_us = skb_mstamp_us_delta(&now, &first_ackt);
		ca_rtt_us = skb_mstamp_us_delta(&now, &last_ackt);
	}
	if (sack->first_sackt.v64) {
		sack_rtt_us = skb_mstamp_us_delta(&now, &sack->first_sackt);
		ca_rtt_us = skb_mstamp_us_delta(&now, &sack->last_sackt);
	}

	rtt_update = tcp_ack_update_rtt(sk, flag, seq_rtt_us, sack_rtt_us,
					ca_rtt_us);

	if (flag & FLAG_ACKED) {
		tcp_rearm_rto(sk);
		if (unlikely(icsk->icsk_mtup.probe_size &&
			     !after(tp->mtu_probe.probe_seq_end, tp->snd_una))) {
			tcp_mtup_probe_success(sk);
		}

		if (tcp_is_reno(tp)) {
			tcp_remove_reno_sacks(sk, pkts_acked);
		} else {
			int delta;

			/* Non-retransmitted hole got filled? That's reordering */
			if (reord < prior_fackets)
				tcp_update_reordering(sk, tp->fackets_out - reord, 0);

			delta = tcp_is_fack(tp) ? pkts_acked :
						  prior_sacked - tp->sacked_out;
			tp->lost_cnt_hint -= min(tp->lost_cnt_hint, delta);
		}

		tp->fackets_out -= min(pkts_acked, tp->fackets_out);

	} else if (skb && rtt_update && sack_rtt_us >= 0 &&
		   sack_rtt_us > skb_mstamp_us_delta(&now, &skb->skb_mstamp)) {
		/* Do not re-arm RTO if the sack RTT is measured from data sent
		 * after when the head was last (re)transmitted. Otherwise the
		 * timeout may continue to extend in loss recovery.
		 */
		tcp_rearm_rto(sk);
	}

	if (icsk->icsk_ca_ops->pkts_acked) {
		struct ack_sample sample = { .pkts_acked = pkts_acked,
					     .rtt_us = ca_rtt_us };

		icsk->icsk_ca_ops->pkts_acked(sk, &sample);
	}

#if FASTRETRANS_DEBUG > 0
	WARN_ON((int)tp->sacked_out < 0);
	WARN_ON((int)tp->lost_out < 0);
	WARN_ON((int)tp->retrans_out < 0);
	if (!tp->packets_out && tcp_is_sack(tp)) {
		icsk = inet_csk(sk);
		if (tp->lost_out) {
			pr_debug("Leak l=%u %d\n",
				 tp->lost_out, icsk->icsk_ca_state);
			tp->lost_out = 0;
		}
		if (tp->sacked_out) {
			pr_debug("Leak s=%u %d\n",
				 tp->sacked_out, icsk->icsk_ca_state);
			tp->sacked_out = 0;
		}
		if (tp->retrans_out) {
			pr_debug("Leak r=%u %d\n",
				 tp->retrans_out, icsk->icsk_ca_state);
			tp->retrans_out = 0;
		}
	}
#endif
	*acked = pkts_acked;
	return flag;
}