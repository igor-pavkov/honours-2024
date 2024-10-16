static ssize_t host_features_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct vbg_dev *gdev = dev_get_drvdata(dev);

	return sprintf(buf, "%#x\n", gdev->host_features);
}