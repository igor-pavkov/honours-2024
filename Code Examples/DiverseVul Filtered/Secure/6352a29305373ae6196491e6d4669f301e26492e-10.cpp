int ecryptfs_parse_packet_length(unsigned char *data, size_t *size,
				 size_t *length_size)
{
	int rc = 0;

	(*length_size) = 0;
	(*size) = 0;
	if (data[0] < 192) {
		/* One-byte length */
		(*size) = (unsigned char)data[0];
		(*length_size) = 1;
	} else if (data[0] < 224) {
		/* Two-byte length */
		(*size) = (((unsigned char)(data[0]) - 192) * 256);
		(*size) += ((unsigned char)(data[1]) + 192);
		(*length_size) = 2;
	} else if (data[0] == 255) {
		/* Five-byte length; we're not supposed to see this */
		ecryptfs_printk(KERN_ERR, "Five-byte packet length not "
				"supported\n");
		rc = -EINVAL;
		goto out;
	} else {
		ecryptfs_printk(KERN_ERR, "Error parsing packet length\n");
		rc = -EINVAL;
		goto out;
	}
out:
	return rc;
}