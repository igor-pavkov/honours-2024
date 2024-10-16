static void ppp_timer(struct timer_list *t)
{
	struct proto *proto = from_timer(proto, t, timer);
	struct ppp *ppp = get_ppp(proto->dev);
	unsigned long flags;

	spin_lock_irqsave(&ppp->lock, flags);
	switch (proto->state) {
	case STOPPING:
	case REQ_SENT:
	case ACK_RECV:
	case ACK_SENT:
		if (proto->restart_counter) {
			ppp_cp_event(proto->dev, proto->pid, TO_GOOD, 0, 0,
				     0, NULL);
			proto->restart_counter--;
		} else if (netif_carrier_ok(proto->dev))
			ppp_cp_event(proto->dev, proto->pid, TO_GOOD, 0, 0,
				     0, NULL);
		else
			ppp_cp_event(proto->dev, proto->pid, TO_BAD, 0, 0,
				     0, NULL);
		break;

	case OPENED:
		if (proto->pid != PID_LCP)
			break;
		if (time_after(jiffies, ppp->last_pong +
			       ppp->keepalive_timeout * HZ)) {
			netdev_info(proto->dev, "Link down\n");
			ppp_cp_event(proto->dev, PID_LCP, STOP, 0, 0, 0, NULL);
			ppp_cp_event(proto->dev, PID_LCP, START, 0, 0, 0, NULL);
		} else {	/* send keep-alive packet */
			ppp->echo_id = ++ppp->seq;
			ppp_tx_cp(proto->dev, PID_LCP, LCP_ECHO_REQ,
				  ppp->echo_id, 0, NULL);
			proto->timer.expires = jiffies +
				ppp->keepalive_interval * HZ;
			add_timer(&proto->timer);
		}
		break;
	}
	spin_unlock_irqrestore(&ppp->lock, flags);
	ppp_tx_flush();
}