static int uvesafb_open(struct fb_info *info, int user)
{
	struct uvesafb_par *par = info->par;
	int cnt = atomic_read(&par->ref_count);
	u8 *buf = NULL;

	if (!cnt && par->vbe_state_size) {
		buf =  uvesafb_vbe_state_save(par);
		if (IS_ERR(buf)) {
			pr_warn("save hardware state failed, error code is %ld!\n",
				PTR_ERR(buf));
		} else {
			par->vbe_state_orig = buf;
		}
	}

	atomic_inc(&par->ref_count);
	return 0;
}