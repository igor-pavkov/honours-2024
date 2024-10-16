static int parallels_probe(const uint8_t *buf, int buf_size, const char *filename)
{
    const struct parallels_header *ph = (const void *)buf;

    if (buf_size < HEADER_SIZE)
	return 0;

    if (!memcmp(ph->magic, HEADER_MAGIC, 16) &&
	(le32_to_cpu(ph->version) == HEADER_VERSION))
	return 100;

    return 0;
}