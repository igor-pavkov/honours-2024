static void ppp_cp_event(struct net_device *dev, u16 pid, u16 event, u8 code,
			 u8 id, unsigned int len, const void *data)
{
	int old_state, action;
	struct ppp *ppp = get_ppp(dev);
	struct proto *proto = get_proto(dev, pid);

	old_state = proto->state;
	BUG_ON(old_state >= STATES);
	BUG_ON(event >= EVENTS);

#if DEBUG_STATE
	printk(KERN_DEBUG "%s: %s ppp_cp_event(%s) %s ...\n", dev->name,
	       proto_name(pid), event_names[event], state_names[proto->state]);
#endif

	action = cp_table[event][old_state];

	proto->state = action & STATE_MASK;
	if (action & (SCR | STR)) /* set Configure-Req/Terminate-Req timer */
		mod_timer(&proto->timer, proto->timeout =
			  jiffies + ppp->req_timeout * HZ);
	if (action & ZRC)
		proto->restart_counter = 0;
	if (action & IRC)
		proto->restart_counter = (proto->state == STOPPING) ?
			ppp->term_retries : ppp->cr_retries;

	if (action & SCR)	/* send Configure-Request */
		ppp_tx_cp(dev, pid, CP_CONF_REQ, proto->cr_id = ++ppp->seq,
			  0, NULL);
	if (action & SCA)	/* send Configure-Ack */
		ppp_tx_cp(dev, pid, CP_CONF_ACK, id, len, data);
	if (action & SCN)	/* send Configure-Nak/Reject */
		ppp_tx_cp(dev, pid, code, id, len, data);
	if (action & STR)	/* send Terminate-Request */
		ppp_tx_cp(dev, pid, CP_TERM_REQ, ++ppp->seq, 0, NULL);
	if (action & STA)	/* send Terminate-Ack */
		ppp_tx_cp(dev, pid, CP_TERM_ACK, id, 0, NULL);
	if (action & SCJ)	/* send Code-Reject */
		ppp_tx_cp(dev, pid, CP_CODE_REJ, ++ppp->seq, len, data);

	if (old_state != OPENED && proto->state == OPENED) {
		netdev_info(dev, "%s up\n", proto_name(pid));
		if (pid == PID_LCP) {
			netif_dormant_off(dev);
			ppp_cp_event(dev, PID_IPCP, START, 0, 0, 0, NULL);
			ppp_cp_event(dev, PID_IPV6CP, START, 0, 0, 0, NULL);
			ppp->last_pong = jiffies;
			mod_timer(&proto->timer, proto->timeout =
				  jiffies + ppp->keepalive_interval * HZ);
		}
	}
	if (old_state == OPENED && proto->state != OPENED) {
		netdev_info(dev, "%s down\n", proto_name(pid));
		if (pid == PID_LCP) {
			netif_dormant_on(dev);
			ppp_cp_event(dev, PID_IPCP, STOP, 0, 0, 0, NULL);
			ppp_cp_event(dev, PID_IPV6CP, STOP, 0, 0, 0, NULL);
		}
	}
	if (old_state != CLOSED && proto->state == CLOSED)
		del_timer(&proto->timer);

#if DEBUG_STATE
	printk(KERN_DEBUG "%s: %s ppp_cp_event(%s) ... %s\n", dev->name,
	       proto_name(pid), event_names[event], state_names[proto->state]);
#endif
}