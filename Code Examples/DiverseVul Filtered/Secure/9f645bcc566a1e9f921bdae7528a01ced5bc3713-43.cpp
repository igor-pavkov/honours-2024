static ssize_t v86d_show(struct device_driver *dev, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", v86d_path);
}