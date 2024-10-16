static int fbiogetputcmap(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	struct fbcmap32 __user *argp = (void __user *)arg;
	struct fbcmap __user *p = compat_alloc_user_space(sizeof(*p));
	u32 addr;
	int ret;

	ret = copy_in_user(p, argp, 2 * sizeof(int));
	ret |= get_user(addr, &argp->red);
	ret |= put_user(compat_ptr(addr), &p->red);
	ret |= get_user(addr, &argp->green);
	ret |= put_user(compat_ptr(addr), &p->green);
	ret |= get_user(addr, &argp->blue);
	ret |= put_user(compat_ptr(addr), &p->blue);
	if (ret)
		return -EFAULT;
	return info->fbops->fb_ioctl(info,
			(cmd == FBIOPUTCMAP32) ?
			FBIOPUTCMAP_SPARC : FBIOGETCMAP_SPARC,
			(unsigned long)p);
}