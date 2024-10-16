static void selinux_cred_getsecid(const struct cred *c, u32 *secid)
{
	*secid = cred_sid(c);
}