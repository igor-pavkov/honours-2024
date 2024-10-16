static ssize_t uvesafb_show_vbe_modes(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *info = platform_get_drvdata(to_platform_device(dev));
	struct uvesafb_par *par = info->par;
	int ret = 0, i;

	for (i = 0; i < par->vbe_modes_cnt && ret < PAGE_SIZE; i++) {
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"%dx%d-%d, 0x%.4x\n",
			par->vbe_modes[i].x_res, par->vbe_modes[i].y_res,
			par->vbe_modes[i].depth, par->vbe_modes[i].mode_id);
	}

	return ret;
}