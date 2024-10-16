static int encrypt_and_sign_nss_2_0 (
	struct crypto_instance *instance,
	const unsigned char *buf_in,
	const size_t buf_in_len,
	unsigned char *buf_out,
	size_t *buf_out_len)
{
	unsigned char	*hash = buf_out;
	unsigned char	*data = hash + hash_len[instance->crypto_hash_type];

	if (encrypt_nss(instance, buf_in, buf_in_len, data, buf_out_len) < 0) {
		return -1;
	}

	if (hash_to_nss[instance->crypto_hash_type]) {
		if (calculate_nss_hash(instance, data, *buf_out_len, hash) < 0) {
			return -1;
		}
		*buf_out_len = *buf_out_len + hash_len[instance->crypto_hash_type];
	}

	return 0;
}