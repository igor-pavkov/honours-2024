static int pipe_do_rcv(struct sock *sk, struct sk_buff *skb)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *hdr = pnp_hdr(skb);
	struct sk_buff_head *queue;
	int err = 0;

	BUG_ON(sk->sk_state == TCP_CLOSE_WAIT);

	switch (hdr->message_id) {
	case PNS_PEP_CONNECT_REQ:
		pep_reject_conn(sk, skb, PN_PIPE_ERR_PEP_IN_USE, GFP_ATOMIC);
		break;

	case PNS_PEP_DISCONNECT_REQ:
		pep_reply(sk, skb, PN_PIPE_NO_ERROR, NULL, 0, GFP_ATOMIC);
		sk->sk_state = TCP_CLOSE_WAIT;
		if (!sock_flag(sk, SOCK_DEAD))
			sk->sk_state_change(sk);
		break;

	case PNS_PEP_ENABLE_REQ:
		/* Wait for PNS_PIPE_(ENABLED|REDIRECTED)_IND */
		pep_reply(sk, skb, PN_PIPE_NO_ERROR, NULL, 0, GFP_ATOMIC);
		break;

	case PNS_PEP_RESET_REQ:
		switch (hdr->state_after_reset) {
		case PN_PIPE_DISABLE:
			pn->init_enable = 0;
			break;
		case PN_PIPE_ENABLE:
			pn->init_enable = 1;
			break;
		default: /* not allowed to send an error here!? */
			err = -EINVAL;
			goto out;
		}
		fallthrough;
	case PNS_PEP_DISABLE_REQ:
		atomic_set(&pn->tx_credits, 0);
		pep_reply(sk, skb, PN_PIPE_NO_ERROR, NULL, 0, GFP_ATOMIC);
		break;

	case PNS_PEP_CTRL_REQ:
		if (skb_queue_len(&pn->ctrlreq_queue) >= PNPIPE_CTRLREQ_MAX) {
			atomic_inc(&sk->sk_drops);
			break;
		}
		__skb_pull(skb, 4);
		queue = &pn->ctrlreq_queue;
		goto queue;

	case PNS_PIPE_ALIGNED_DATA:
		__skb_pull(skb, 1);
		fallthrough;
	case PNS_PIPE_DATA:
		__skb_pull(skb, 3); /* Pipe data header */
		if (!pn_flow_safe(pn->rx_fc)) {
			err = sock_queue_rcv_skb(sk, skb);
			if (!err)
				return NET_RX_SUCCESS;
			err = -ENOBUFS;
			break;
		}

		if (pn->rx_credits == 0) {
			atomic_inc(&sk->sk_drops);
			err = -ENOBUFS;
			break;
		}
		pn->rx_credits--;
		queue = &sk->sk_receive_queue;
		goto queue;

	case PNS_PEP_STATUS_IND:
		pipe_rcv_status(sk, skb);
		break;

	case PNS_PIPE_REDIRECTED_IND:
		err = pipe_rcv_created(sk, skb);
		break;

	case PNS_PIPE_CREATED_IND:
		err = pipe_rcv_created(sk, skb);
		if (err)
			break;
		fallthrough;
	case PNS_PIPE_RESET_IND:
		if (!pn->init_enable)
			break;
		fallthrough;
	case PNS_PIPE_ENABLED_IND:
		if (!pn_flow_safe(pn->tx_fc)) {
			atomic_set(&pn->tx_credits, 1);
			sk->sk_write_space(sk);
		}
		if (sk->sk_state == TCP_ESTABLISHED)
			break; /* Nothing to do */
		sk->sk_state = TCP_ESTABLISHED;
		pipe_grant_credits(sk, GFP_ATOMIC);
		break;

	case PNS_PIPE_DISABLED_IND:
		sk->sk_state = TCP_SYN_RECV;
		pn->rx_credits = 0;
		break;

	default:
		net_dbg_ratelimited("Phonet unknown PEP message: %u\n",
				    hdr->message_id);
		err = -EINVAL;
	}
out:
	kfree_skb(skb);
	return (err == -ENOBUFS) ? NET_RX_DROP : NET_RX_SUCCESS;

queue:
	skb->dev = NULL;
	skb_set_owner_r(skb, sk);
	skb_queue_tail(queue, skb);
	if (!sock_flag(sk, SOCK_DEAD))
		sk->sk_data_ready(sk);
	return NET_RX_SUCCESS;
}