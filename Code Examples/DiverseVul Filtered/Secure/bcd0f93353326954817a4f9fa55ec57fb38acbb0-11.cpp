static int pipe_handler_do_rcv(struct sock *sk, struct sk_buff *skb)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *hdr = pnp_hdr(skb);
	int err = NET_RX_SUCCESS;

	switch (hdr->message_id) {
	case PNS_PIPE_ALIGNED_DATA:
		__skb_pull(skb, 1);
		fallthrough;
	case PNS_PIPE_DATA:
		__skb_pull(skb, 3); /* Pipe data header */
		if (!pn_flow_safe(pn->rx_fc)) {
			err = sock_queue_rcv_skb(sk, skb);
			if (!err)
				return NET_RX_SUCCESS;
			err = NET_RX_DROP;
			break;
		}

		if (pn->rx_credits == 0) {
			atomic_inc(&sk->sk_drops);
			err = NET_RX_DROP;
			break;
		}
		pn->rx_credits--;
		skb->dev = NULL;
		skb_set_owner_r(skb, sk);
		skb_queue_tail(&sk->sk_receive_queue, skb);
		if (!sock_flag(sk, SOCK_DEAD))
			sk->sk_data_ready(sk);
		return NET_RX_SUCCESS;

	case PNS_PEP_CONNECT_RESP:
		if (sk->sk_state != TCP_SYN_SENT)
			break;
		if (!sock_flag(sk, SOCK_DEAD))
			sk->sk_state_change(sk);
		if (pep_connresp_rcv(sk, skb)) {
			sk->sk_state = TCP_CLOSE_WAIT;
			break;
		}
		if (pn->init_enable == PN_PIPE_DISABLE)
			sk->sk_state = TCP_SYN_RECV;
		else {
			sk->sk_state = TCP_ESTABLISHED;
			pipe_start_flow_control(sk);
		}
		break;

	case PNS_PEP_ENABLE_RESP:
		if (sk->sk_state != TCP_SYN_SENT)
			break;

		if (pep_enableresp_rcv(sk, skb)) {
			sk->sk_state = TCP_CLOSE_WAIT;
			break;
		}

		sk->sk_state = TCP_ESTABLISHED;
		pipe_start_flow_control(sk);
		break;

	case PNS_PEP_DISCONNECT_RESP:
		/* sock should already be dead, nothing to do */
		break;

	case PNS_PEP_STATUS_IND:
		pipe_rcv_status(sk, skb);
		break;
	}
	kfree_skb(skb);
	return err;
}