static int vivid_fb_ioctl(struct fb_info *info, unsigned cmd, unsigned long arg)
{
	struct vivid_dev *dev = (struct vivid_dev *)info->par;

	switch (cmd) {
	case FBIOGET_VBLANK: {
		struct fb_vblank vblank;

		memset(&vblank, 0, sizeof(vblank));
		vblank.flags = FB_VBLANK_HAVE_COUNT | FB_VBLANK_HAVE_VCOUNT |
			FB_VBLANK_HAVE_VSYNC;
		vblank.count = 0;
		vblank.vcount = 0;
		vblank.hcount = 0;
		if (copy_to_user((void __user *)arg, &vblank, sizeof(vblank)))
			return -EFAULT;
		return 0;
	}

	default:
		dprintk(dev, 1, "Unknown ioctl %08x\n", cmd);
		return -EINVAL;
	}
	return 0;
}