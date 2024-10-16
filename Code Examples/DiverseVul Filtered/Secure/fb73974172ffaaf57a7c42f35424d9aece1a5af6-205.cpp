static int selinux_tun_dev_create(void)
{
	u32 sid = current_sid();

	/* we aren't taking into account the "sockcreate" SID since the socket
	 * that is being created here is not a socket in the traditional sense,
	 * instead it is a private sock, accessible only to the kernel, and
	 * representing a wide range of network traffic spanning multiple
	 * connections unlike traditional sockets - check the TUN driver to
	 * get a better understanding of why this socket is special */

	return avc_has_perm(&selinux_state,
			    sid, sid, SECCLASS_TUN_SOCKET, TUN_SOCKET__CREATE,
			    NULL);
}