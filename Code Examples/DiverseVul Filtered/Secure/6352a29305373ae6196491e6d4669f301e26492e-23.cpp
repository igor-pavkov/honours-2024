static int ecryptfs_verify_version(u16 version)
{
	int rc = 0;
	unsigned char major;
	unsigned char minor;

	major = ((version >> 8) & 0xFF);
	minor = (version & 0xFF);
	if (major != ECRYPTFS_VERSION_MAJOR) {
		ecryptfs_printk(KERN_ERR, "Major version number mismatch. "
				"Expected [%d]; got [%d]\n",
				ECRYPTFS_VERSION_MAJOR, major);
		rc = -EINVAL;
		goto out;
	}
	if (minor != ECRYPTFS_VERSION_MINOR) {
		ecryptfs_printk(KERN_ERR, "Minor version number mismatch. "
				"Expected [%d]; got [%d]\n",
				ECRYPTFS_VERSION_MINOR, minor);
		rc = -EINVAL;
		goto out;
	}
out:
	return rc;
}