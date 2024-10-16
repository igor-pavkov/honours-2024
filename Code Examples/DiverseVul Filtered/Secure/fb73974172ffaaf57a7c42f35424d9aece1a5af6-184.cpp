static void selinux_ipc_getsecid(struct kern_ipc_perm *ipcp, u32 *secid)
{
	struct ipc_security_struct *isec = selinux_ipc(ipcp);
	*secid = isec->sid;
}