static int llc_ui_send_data(struct sock* sk, struct sk_buff *skb, int noblock)
{
	struct llc_sock* llc = llc_sk(sk);
	int rc = 0;

	if (unlikely(llc_data_accept_state(llc->state) ||
		     llc->remote_busy_flag ||
		     llc->p_flag)) {
		long timeout = sock_sndtimeo(sk, noblock);

		rc = llc_ui_wait_for_busy_core(sk, timeout);
	}
	if (unlikely(!rc))
		rc = llc_build_and_send_pkt(sk, skb);
	return rc;
}