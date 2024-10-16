static int process_request_key_err(long err_code)
{
	int rc = 0;

	switch (err_code) {
	case -ENOKEY:
		ecryptfs_printk(KERN_WARNING, "No key\n");
		rc = -ENOENT;
		break;
	case -EKEYEXPIRED:
		ecryptfs_printk(KERN_WARNING, "Key expired\n");
		rc = -ETIME;
		break;
	case -EKEYREVOKED:
		ecryptfs_printk(KERN_WARNING, "Key revoked\n");
		rc = -EINVAL;
		break;
	default:
		ecryptfs_printk(KERN_WARNING, "Unknown error code: "
				"[0x%.16x]\n", err_code);
		rc = -EINVAL;
	}
	return rc;
}