static int irda_compat_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	/*
	 * All IRDA's ioctl are standard ones.
	 */
	return -ENOIOCTLCMD;
}