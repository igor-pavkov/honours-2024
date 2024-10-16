static inline int verify_replay(struct xfrm_usersa_info *p,
				struct nlattr **attrs)
{
	struct nlattr *rt = attrs[XFRMA_REPLAY_ESN_VAL];
	struct xfrm_replay_state_esn *rs;

	if (!rt)
		return (p->flags & XFRM_STATE_ESN) ? -EINVAL : 0;

	rs = nla_data(rt);

	if (rs->bmp_len > XFRMA_REPLAY_ESN_MAX / sizeof(rs->bmp[0]) / 8)
		return -EINVAL;

	if (nla_len(rt) < (int)xfrm_replay_state_esn_len(rs) &&
	    nla_len(rt) != sizeof(*rs))
		return -EINVAL;

	/* As only ESP and AH support ESN feature. */
	if ((p->id.proto != IPPROTO_ESP) && (p->id.proto != IPPROTO_AH))
		return -EINVAL;

	if (p->replay_window != 0)
		return -EINVAL;

	return 0;
}