static int mount_usage(FILE *stream)
{
	int rc;

	if (strcmp(thisprogram, "mount.smb2") == 0)
		rc = mount_smb2_usage(stream);
	else
		rc = mount_cifs_usage(stream);

	return rc;
}