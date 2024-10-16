static int selinux_tun_dev_attach_queue(void *security)
{
	struct tun_security_struct *tunsec = security;

	return avc_has_perm(current_sid(), tunsec->sid, SECCLASS_TUN_SOCKET,
			    TUN_SOCKET__ATTACH_QUEUE, NULL);
}