static inline unsigned int xfrm_mapping_msgsize(void)
{
	return NLMSG_ALIGN(sizeof(struct xfrm_user_mapping));
}