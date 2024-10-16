static int selinux_tun_dev_open(void *security)
{
	struct tun_security_struct *tunsec = security;
	u32 sid = current_sid();
	int err;

	err = avc_has_perm(sid, tunsec->sid, SECCLASS_TUN_SOCKET,
			   TUN_SOCKET__RELABELFROM, NULL);
	if (err)
		return err;
	err = avc_has_perm(sid, sid, SECCLASS_TUN_SOCKET,
			   TUN_SOCKET__RELABELTO, NULL);
	if (err)
		return err;
	tunsec->sid = sid;

	return 0;
}