static int rfcomm_tty_ioctl(struct tty_struct *tty, unsigned int cmd, unsigned long arg)
{
	BT_DBG("tty %p cmd 0x%02x", tty, cmd);

	switch (cmd) {
	case TCGETS:
		BT_DBG("TCGETS is not supported");
		return -ENOIOCTLCMD;

	case TCSETS:
		BT_DBG("TCSETS is not supported");
		return -ENOIOCTLCMD;

	case TIOCMIWAIT:
		BT_DBG("TIOCMIWAIT");
		break;

	case TIOCGSERIAL:
		BT_ERR("TIOCGSERIAL is not supported");
		return -ENOIOCTLCMD;

	case TIOCSSERIAL:
		BT_ERR("TIOCSSERIAL is not supported");
		return -ENOIOCTLCMD;

	case TIOCSERGSTRUCT:
		BT_ERR("TIOCSERGSTRUCT is not supported");
		return -ENOIOCTLCMD;

	case TIOCSERGETLSR:
		BT_ERR("TIOCSERGETLSR is not supported");
		return -ENOIOCTLCMD;

	case TIOCSERCONFIG:
		BT_ERR("TIOCSERCONFIG is not supported");
		return -ENOIOCTLCMD;

	default:
		return -ENOIOCTLCMD;	/* ioctls which we must ignore */

	}

	return -ENOIOCTLCMD;
}