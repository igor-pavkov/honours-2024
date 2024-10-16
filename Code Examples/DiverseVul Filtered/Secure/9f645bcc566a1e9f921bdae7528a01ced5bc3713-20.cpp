static ssize_t v86d_store(struct device_driver *dev, const char *buf,
		size_t count)
{
	strncpy(v86d_path, buf, PATH_MAX);
	return count;
}