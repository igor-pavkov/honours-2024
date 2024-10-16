static ssize_t show_version(struct device *cd,
			    struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", USBVISION_VERSION_STRING);
}