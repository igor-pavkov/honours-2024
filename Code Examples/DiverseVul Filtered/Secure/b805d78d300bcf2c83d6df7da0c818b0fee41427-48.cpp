static inline unsigned int userpolicy_type_attrsize(void)
{
#ifdef CONFIG_XFRM_SUB_POLICY
	return nla_total_size(sizeof(struct xfrm_userpolicy_type));
#else
	return 0;
#endif
}