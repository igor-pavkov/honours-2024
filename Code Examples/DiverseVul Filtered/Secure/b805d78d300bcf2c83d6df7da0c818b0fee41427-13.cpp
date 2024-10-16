static inline unsigned int xfrm_report_msgsize(void)
{
	return NLMSG_ALIGN(sizeof(struct xfrm_user_report));
}