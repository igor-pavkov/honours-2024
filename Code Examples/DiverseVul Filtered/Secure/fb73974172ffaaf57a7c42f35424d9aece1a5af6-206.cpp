static int selinux_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
{
	return security_sid_to_context(&selinux_state, secid,
				       secdata, seclen);
}