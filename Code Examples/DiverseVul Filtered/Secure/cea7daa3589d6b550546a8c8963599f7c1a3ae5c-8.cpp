static inline unsigned keyring_hash(const char *desc)
{
	unsigned bucket = 0;

	for (; *desc; desc++)
		bucket += (unsigned char) *desc;

	return bucket & (KEYRING_NAME_HASH_SIZE - 1);
}