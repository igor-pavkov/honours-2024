int sbusfb_compat_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case FBIOGTYPE:
	case FBIOSATTR:
	case FBIOGATTR:
	case FBIOSVIDEO:
	case FBIOGVIDEO:
	case FBIOGCURSOR32:	/* This is not implemented yet.
				   Later it should be converted... */
	case FBIOSCURPOS:
	case FBIOGCURPOS:
	case FBIOGCURMAX:
		return info->fbops->fb_ioctl(info, cmd, arg);
	case FBIOPUTCMAP32:
		return fbiogetputcmap(info, cmd, arg);
	case FBIOGETCMAP32:
		return fbiogetputcmap(info, cmd, arg);
	case FBIOSCURSOR32:
		return fbiogscursor(info, arg);
	default:
		return -ENOIOCTLCMD;
	}
}