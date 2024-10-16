static int ppp_rx(struct sk_buff *skb)
{
	struct hdlc_header *hdr = (struct hdlc_header*)skb->data;
	struct net_device *dev = skb->dev;
	struct ppp *ppp = get_ppp(dev);
	struct proto *proto;
	struct cp_header *cp;
	unsigned long flags;
	unsigned int len;
	u16 pid;
#if DEBUG_CP
	int i;
	char *ptr;
#endif

	spin_lock_irqsave(&ppp->lock, flags);
	/* Check HDLC header */
	if (skb->len < sizeof(struct hdlc_header))
		goto rx_error;
	cp = skb_pull(skb, sizeof(struct hdlc_header));
	if (hdr->address != HDLC_ADDR_ALLSTATIONS ||
	    hdr->control != HDLC_CTRL_UI)
		goto rx_error;

	pid = ntohs(hdr->protocol);
	proto = get_proto(dev, pid);
	if (!proto) {
		if (ppp->protos[IDX_LCP].state == OPENED)
			ppp_tx_cp(dev, PID_LCP, LCP_PROTO_REJ,
				  ++ppp->seq, skb->len + 2, &hdr->protocol);
		goto rx_error;
	}

	len = ntohs(cp->len);
	if (len < sizeof(struct cp_header) /* no complete CP header? */ ||
	    skb->len < len /* truncated packet? */)
		goto rx_error;
	skb_pull(skb, sizeof(struct cp_header));
	len -= sizeof(struct cp_header);

	/* HDLC and CP headers stripped from skb */
#if DEBUG_CP
	if (cp->code < CP_CODES)
		sprintf(debug_buffer, "[%s id 0x%X]", code_names[cp->code],
			cp->id);
	else
		sprintf(debug_buffer, "[code %u id 0x%X]", cp->code, cp->id);
	ptr = debug_buffer + strlen(debug_buffer);
	for (i = 0; i < min_t(unsigned int, len, DEBUG_CP); i++) {
		sprintf(ptr, " %02X", skb->data[i]);
		ptr += strlen(ptr);
	}
	printk(KERN_DEBUG "%s: RX %s %s\n", dev->name, proto_name(pid),
	       debug_buffer);
#endif

	/* LCP only */
	if (pid == PID_LCP)
		switch (cp->code) {
		case LCP_PROTO_REJ:
			pid = ntohs(*(__be16*)skb->data);
			if (pid == PID_LCP || pid == PID_IPCP ||
			    pid == PID_IPV6CP)
				ppp_cp_event(dev, pid, RXJ_BAD, 0, 0,
					     0, NULL);
			goto out;

		case LCP_ECHO_REQ: /* send Echo-Reply */
			if (len >= 4 && proto->state == OPENED)
				ppp_tx_cp(dev, PID_LCP, LCP_ECHO_REPLY,
					  cp->id, len - 4, skb->data + 4);
			goto out;

		case LCP_ECHO_REPLY:
			if (cp->id == ppp->echo_id)
				ppp->last_pong = jiffies;
			goto out;

		case LCP_DISC_REQ: /* discard */
			goto out;
		}

	/* LCP, IPCP and IPV6CP */
	switch (cp->code) {
	case CP_CONF_REQ:
		ppp_cp_parse_cr(dev, pid, cp->id, len, skb->data);
		break;

	case CP_CONF_ACK:
		if (cp->id == proto->cr_id)
			ppp_cp_event(dev, pid, RCA, 0, 0, 0, NULL);
		break;

	case CP_CONF_REJ:
	case CP_CONF_NAK:
		if (cp->id == proto->cr_id)
			ppp_cp_event(dev, pid, RCN, 0, 0, 0, NULL);
		break;

	case CP_TERM_REQ:
		ppp_cp_event(dev, pid, RTR, 0, cp->id, 0, NULL);
		break;

	case CP_TERM_ACK:
		ppp_cp_event(dev, pid, RTA, 0, 0, 0, NULL);
		break;

	case CP_CODE_REJ:
		ppp_cp_event(dev, pid, RXJ_BAD, 0, 0, 0, NULL);
		break;

	default:
		len += sizeof(struct cp_header);
		if (len > dev->mtu)
			len = dev->mtu;
		ppp_cp_event(dev, pid, RUC, 0, 0, len, cp);
		break;
	}
	goto out;

rx_error:
	dev->stats.rx_errors++;
out:
	spin_unlock_irqrestore(&ppp->lock, flags);
	dev_kfree_skb_any(skb);
	ppp_tx_flush();
	return NET_RX_DROP;
}