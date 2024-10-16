static ssize_t uvesafb_show_product_name(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *info = platform_get_drvdata(to_platform_device(dev));
	struct uvesafb_par *par = info->par;

	if (par->vbe_ib.oem_product_name_ptr)
		return snprintf(buf, PAGE_SIZE, "%s\n", (char *)
			(&par->vbe_ib) + par->vbe_ib.oem_product_name_ptr);
	else
		return 0;
}