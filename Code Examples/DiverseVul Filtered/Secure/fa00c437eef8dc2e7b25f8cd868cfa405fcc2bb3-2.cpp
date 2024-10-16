static int check_revision(struct aac_dev *dev, void __user *arg)
{
	struct revision response;
	char *driver_version = aac_driver_version;
	u32 version;

	response.compat = 1;
	version = (simple_strtol(driver_version,
				&driver_version, 10) << 24) | 0x00000400;
	version += simple_strtol(driver_version + 1, &driver_version, 10) << 16;
	version += simple_strtol(driver_version + 1, NULL, 10);
	response.version = cpu_to_le32(version);
#	ifdef AAC_DRIVER_BUILD
		response.build = cpu_to_le32(AAC_DRIVER_BUILD);
#	else
		response.build = cpu_to_le32(9999);
#	endif

	if (copy_to_user(arg, &response, sizeof(response)))
		return -EFAULT;
	return 0;
}