static void copy_to_user_state(struct xfrm_state *x, struct xfrm_usersa_info *p)
{
	memset(p, 0, sizeof(*p));
	memcpy(&p->id, &x->id, sizeof(p->id));
	memcpy(&p->sel, &x->sel, sizeof(p->sel));
	memcpy(&p->lft, &x->lft, sizeof(p->lft));
	memcpy(&p->curlft, &x->curlft, sizeof(p->curlft));
	put_unaligned(x->stats.replay_window, &p->stats.replay_window);
	put_unaligned(x->stats.replay, &p->stats.replay);
	put_unaligned(x->stats.integrity_failed, &p->stats.integrity_failed);
	memcpy(&p->saddr, &x->props.saddr, sizeof(p->saddr));
	p->mode = x->props.mode;
	p->replay_window = x->props.replay_window;
	p->reqid = x->props.reqid;
	p->family = x->props.family;
	p->flags = x->props.flags;
	p->seq = x->km.seq;
}