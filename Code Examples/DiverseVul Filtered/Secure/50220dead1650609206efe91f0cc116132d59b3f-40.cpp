static ssize_t modalias_show(struct device *dev, struct device_attribute *a,
			     char *buf)
{
	struct hid_device *hdev = container_of(dev, struct hid_device, dev);

	return scnprintf(buf, PAGE_SIZE, "hid:b%04Xg%04Xv%08Xp%08X\n",
			 hdev->bus, hdev->group, hdev->vendor, hdev->product);
}