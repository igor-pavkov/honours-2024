static inline unsigned int xfrm_aevent_msgsize(struct xfrm_state *x)
{
	unsigned int replay_size = x->replay_esn ?
			      xfrm_replay_state_esn_len(x->replay_esn) :
			      sizeof(struct xfrm_replay_state);

	return NLMSG_ALIGN(sizeof(struct xfrm_aevent_id))
	       + nla_total_size(replay_size)
	       + nla_total_size_64bit(sizeof(struct xfrm_lifetime_cur))
	       + nla_total_size(sizeof(struct xfrm_mark))
	       + nla_total_size(4) /* XFRM_AE_RTHR */
	       + nla_total_size(4); /* XFRM_AE_ETHR */
}