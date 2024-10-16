PJ_DEF(void) pjmedia_rtcp_build_rtcp(pjmedia_rtcp_session *sess, 
				     void **ret_p_pkt, int *len)
{
    pj_uint32_t expected, expected_interval, received_interval, lost_interval;
    pjmedia_rtcp_sr *sr;
    pjmedia_rtcp_rr *rr;
    pj_timestamp ts_now;
    pjmedia_rtcp_ntp_rec ntp;

    /* Get current NTP time. */
    pj_get_timestamp(&ts_now);
    pjmedia_rtcp_get_ntp_time(sess, &ntp);


    /* See if we have transmitted RTP packets since last time we
     * sent RTCP SR.
     */
    if (sess->stat.tx.pkt != pj_ntohl(sess->rtcp_sr_pkt.sr.sender_pcount)) {
	pj_time_val ts_time;
	pj_uint32_t rtp_ts;

	/* So we should send RTCP SR */
	*ret_p_pkt = (void*) &sess->rtcp_sr_pkt;
	*len = sizeof(pjmedia_rtcp_sr_pkt);
	rr = &sess->rtcp_sr_pkt.rr;
	sr = &sess->rtcp_sr_pkt.sr;

	/* Update packet count */
	sr->sender_pcount = pj_htonl(sess->stat.tx.pkt);

	/* Update octets count */
	sr->sender_bcount = pj_htonl(sess->stat.tx.bytes);

	/* Fill in NTP timestamp in SR. */
	sr->ntp_sec = pj_htonl(ntp.hi);
	sr->ntp_frac = pj_htonl(ntp.lo);

	/* Fill in RTP timestamp (corresponds to NTP timestamp) in SR. */
	ts_time.sec = ntp.hi - sess->tv_base.sec - JAN_1970;
	ts_time.msec = (long)(ntp.lo * 1000.0 / 0xFFFFFFFF);
	rtp_ts = sess->rtp_ts_base +
		 (pj_uint32_t)(sess->clock_rate*ts_time.sec) +
		 (pj_uint32_t)(sess->clock_rate*ts_time.msec/1000);
	sr->rtp_ts = pj_htonl(rtp_ts);

	TRACE_((sess->name, "TX RTCP SR: ntp_ts=%p", 
			   ((ntp.hi & 0xFFFF) << 16) + ((ntp.lo & 0xFFFF0000) 
				>> 16)));


    } else {
	/* We should send RTCP RR then */
	*ret_p_pkt = (void*) &sess->rtcp_rr_pkt;
	*len = sizeof(pjmedia_rtcp_rr_pkt);
	rr = &sess->rtcp_rr_pkt.rr;
	sr = NULL;
    }
    
    /* SSRC and last_seq */
    rr->ssrc = pj_htonl(sess->peer_ssrc);
    rr->last_seq = (sess->seq_ctrl.cycles & 0xFFFF0000L);
    /* Since this is an "+=" operation, make sure we update last_seq on
     * both RR and SR.
     */
    sess->rtcp_sr_pkt.rr.last_seq += sess->seq_ctrl.max_seq;
    sess->rtcp_rr_pkt.rr.last_seq += sess->seq_ctrl.max_seq;
    rr->last_seq = pj_htonl(rr->last_seq);


    /* Jitter */
    rr->jitter = pj_htonl(sess->jitter >> 4);
    
    
    /* Total lost. */
    expected = pj_ntohl(rr->last_seq) - sess->seq_ctrl.base_seq;

    /* This is bug: total lost already calculated on each incoming RTP!
    if (expected >= sess->received)
	sess->stat.rx.loss = expected - sess->received;
    else
	sess->stat.rx.loss = 0;
    */

    rr->total_lost_2 = (sess->stat.rx.loss >> 16) & 0xFF;
    rr->total_lost_1 = (sess->stat.rx.loss >> 8) & 0xFF;
    rr->total_lost_0 = (sess->stat.rx.loss & 0xFF);

    /* Fraction lost calculation */
    expected_interval = expected - sess->exp_prior;
    sess->exp_prior = expected;
    
    received_interval = sess->received - sess->rx_prior;
    sess->rx_prior = sess->received;
    
    if (expected_interval >= received_interval)
	lost_interval = expected_interval - received_interval;
    else
	lost_interval = 0;
    
    if (expected_interval==0 || lost_interval == 0) {
	rr->fract_lost = 0;
    } else {
	rr->fract_lost = (lost_interval << 8) / expected_interval;
    }
    
    if (sess->rx_lsr_time.u64 == 0 || sess->rx_lsr == 0) {
	rr->lsr = 0;
	rr->dlsr = 0;
    } else {
	pj_timestamp ts;
	pj_uint32_t lsr = sess->rx_lsr;
	pj_uint64_t lsr_time = sess->rx_lsr_time.u64;
	pj_uint32_t dlsr;
	
	/* Convert LSR time to 1/65536 seconds resolution */
	lsr_time = (lsr_time << 16) / sess->ts_freq.u64;

	/* Fill in LSR.
	   LSR is the middle 32bit of the last SR NTP time received.
	 */
	rr->lsr = pj_htonl(lsr);
	
	/* Fill in DLSR.
	   DLSR is Delay since Last SR, in 1/65536 seconds.
	 */
	ts.u64 = ts_now.u64;

	/* Convert interval to 1/65536 seconds value */
	ts.u64 = (ts.u64 << 16) / sess->ts_freq.u64;

	/* Get DLSR */
	dlsr = (pj_uint32_t)(ts.u64 - lsr_time);
	rr->dlsr = pj_htonl(dlsr);

	TRACE_((sess->name,"Tx RTCP RR: lsr=%p, lsr_time=%p, now=%p, dlsr=%p"
			   "(%ds:%03dms)",
			   lsr, 
			   (pj_uint32_t)lsr_time,
			   (pj_uint32_t)ts.u64, 
			   dlsr,
			   dlsr/65536,
			   (dlsr%65536)*1000/65536 ));
    }
    
    /* Update counter */
    pj_gettimeofday(&sess->stat.rx.update);
    sess->stat.rx.update_cnt++;
}