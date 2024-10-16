static int fbiogscursor(struct fb_info *info, unsigned long arg)
{
	struct fbcursor __user *p = compat_alloc_user_space(sizeof(*p));
	struct fbcursor32 __user *argp =  (void __user *)arg;
	compat_uptr_t addr;
	int ret;

	ret = copy_in_user(p, argp,
			      2 * sizeof (short) + 2 * sizeof(struct fbcurpos));
	ret |= copy_in_user(&p->size, &argp->size, sizeof(struct fbcurpos));
	ret |= copy_in_user(&p->cmap, &argp->cmap, 2 * sizeof(int));
	ret |= get_user(addr, &argp->cmap.red);
	ret |= put_user(compat_ptr(addr), &p->cmap.red);
	ret |= get_user(addr, &argp->cmap.green);
	ret |= put_user(compat_ptr(addr), &p->cmap.green);
	ret |= get_user(addr, &argp->cmap.blue);
	ret |= put_user(compat_ptr(addr), &p->cmap.blue);
	ret |= get_user(addr, &argp->mask);
	ret |= put_user(compat_ptr(addr), &p->mask);
	ret |= get_user(addr, &argp->image);
	ret |= put_user(compat_ptr(addr), &p->image);
	if (ret)
		return -EFAULT;
	return info->fbops->fb_ioctl(info, FBIOSCURSOR, (unsigned long)p);
}