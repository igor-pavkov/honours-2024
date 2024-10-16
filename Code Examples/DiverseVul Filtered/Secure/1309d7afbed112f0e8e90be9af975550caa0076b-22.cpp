ssize_t tpm_store_cancel(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);
	if (chip == NULL)
		return 0;

	chip->vendor.cancel(chip);
	return count;
}