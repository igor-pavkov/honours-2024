static inline unsigned int xfrm_expire_msgsize(void)
{
	return NLMSG_ALIGN(sizeof(struct xfrm_user_expire))
	       + nla_total_size(sizeof(struct xfrm_mark));
}