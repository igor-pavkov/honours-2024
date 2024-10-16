void tpm_dev_vendor_release(struct tpm_chip *chip)
{
	if (chip->vendor.release)
		chip->vendor.release(chip->dev);

	clear_bit(chip->dev_num, dev_mask);
	kfree(chip->vendor.miscdev.name);
}