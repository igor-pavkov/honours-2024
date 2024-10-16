void tcp_openreq_init_rwin(struct request_sock *req,
			   const struct sock *sk_listener,
			   const struct dst_entry *dst)
{
	struct inet_request_sock *ireq = inet_rsk(req);
	const struct tcp_sock *tp = tcp_sk(sk_listener);
	int full_space = tcp_full_space(sk_listener);
	u32 window_clamp;
	__u8 rcv_wscale;
	int mss;

	mss = tcp_mss_clamp(tp, dst_metric_advmss(dst));
	window_clamp = READ_ONCE(tp->window_clamp);
	/* Set this up on the first call only */
	req->rsk_window_clamp = window_clamp ? : dst_metric(dst, RTAX_WINDOW);

	/* limit the window selection if the user enforce a smaller rx buffer */
	if (sk_listener->sk_userlocks & SOCK_RCVBUF_LOCK &&
	    (req->rsk_window_clamp > full_space || req->rsk_window_clamp == 0))
		req->rsk_window_clamp = full_space;

	/* tcp_full_space because it is guaranteed to be the first packet */
	tcp_select_initial_window(full_space,
		mss - (ireq->tstamp_ok ? TCPOLEN_TSTAMP_ALIGNED : 0),
		&req->rsk_rcv_wnd,
		&req->rsk_window_clamp,
		ireq->wscale_ok,
		&rcv_wscale,
		dst_metric(dst, RTAX_INITRWND));
	ireq->rcv_wscale = rcv_wscale;
}