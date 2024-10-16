void tpm_dev_release(struct device *dev)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);

	tpm_dev_vendor_release(chip);

	chip->release(dev);
	kfree(chip);
}