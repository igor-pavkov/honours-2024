static int atalk_compat_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	/*
	 * All Appletalk ioctls except SIOCATALKDIFADDR are standard.  And
	 * SIOCATALKDIFADDR is handled by upper layer as well, so there is
	 * nothing to do.  Eventually SIOCATALKDIFADDR should be moved
	 * here so there is no generic SIOCPROTOPRIVATE translation in the
	 * system.
	 */
	return -ENOIOCTLCMD;
}