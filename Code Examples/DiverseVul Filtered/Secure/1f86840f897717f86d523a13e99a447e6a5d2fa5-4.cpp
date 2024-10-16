static int xfrm_new_ae(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_state *x;
	struct km_event c;
	int err = - EINVAL;
	u32 mark = 0;
	struct xfrm_mark m;
	struct xfrm_aevent_id *p = nlmsg_data(nlh);
	struct nlattr *rp = attrs[XFRMA_REPLAY_VAL];
	struct nlattr *re = attrs[XFRMA_REPLAY_ESN_VAL];
	struct nlattr *lt = attrs[XFRMA_LTIME_VAL];

	if (!lt && !rp && !re)
		return err;

	/* pedantic mode - thou shalt sayeth replaceth */
	if (!(nlh->nlmsg_flags&NLM_F_REPLACE))
		return err;

	mark = xfrm_mark_get(attrs, &m);

	x = xfrm_state_lookup(net, mark, &p->sa_id.daddr, p->sa_id.spi, p->sa_id.proto, p->sa_id.family);
	if (x == NULL)
		return -ESRCH;

	if (x->km.state != XFRM_STATE_VALID)
		goto out;

	err = xfrm_replay_verify_len(x->replay_esn, rp);
	if (err)
		goto out;

	spin_lock_bh(&x->lock);
	xfrm_update_ae_params(x, attrs);
	spin_unlock_bh(&x->lock);

	c.event = nlh->nlmsg_type;
	c.seq = nlh->nlmsg_seq;
	c.pid = nlh->nlmsg_pid;
	c.data.aevent = XFRM_AE_CU;
	km_state_notify(x, &c);
	err = 0;
out:
	xfrm_state_put(x);
	return err;
}