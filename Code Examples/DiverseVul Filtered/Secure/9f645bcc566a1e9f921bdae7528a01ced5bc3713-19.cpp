static ssize_t uvesafb_store_nocrtc(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct fb_info *info = platform_get_drvdata(to_platform_device(dev));
	struct uvesafb_par *par = info->par;

	if (count > 0) {
		if (buf[0] == '0')
			par->nocrtc = 0;
		else
			par->nocrtc = 1;
	}
	return count;
}