static php_uint32 phar_tar_checksum(char *buf, int len) /* {{{ */
{
	php_uint32 sum = 0;
	char *end = buf + len;

	while (buf != end) {
		sum += (unsigned char)*buf;
		++buf;
	}
	return sum;
}