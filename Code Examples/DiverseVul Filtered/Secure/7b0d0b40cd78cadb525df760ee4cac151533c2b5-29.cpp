static int selinux_secctx_to_secid(const char *secdata, u32 seclen, u32 *secid)
{
	return security_context_to_sid(secdata, seclen, secid, GFP_KERNEL);
}