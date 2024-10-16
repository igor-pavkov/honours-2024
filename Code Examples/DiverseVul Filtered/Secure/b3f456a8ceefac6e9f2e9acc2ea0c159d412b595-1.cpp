struct crypto_instance *crypto_init(
	const unsigned char *private_key,
	unsigned int private_key_len,
	const char *crypto_cipher_type,
	const char *crypto_hash_type,
	const char *crypto_compat_type,
	void (*log_printf_func) (
		int level,
		int subsys,
                const char *function,
                const char *file,
                int line,
                const char *format,
                ...)__attribute__((format(printf, 6, 7))),
	int log_level_security,
	int log_level_notice,
	int log_level_error,
	int log_subsys_id)
{
	struct crypto_instance *instance;
	instance = malloc(sizeof(*instance));
	if (instance == NULL) {
		return (NULL);
	}
	memset(instance, 0, sizeof(struct crypto_instance));

	memcpy(instance->private_key, private_key, private_key_len);
	instance->private_key_len = private_key_len;

	instance->crypto_cipher_type = string_to_crypto_cipher_type(crypto_cipher_type);
	instance->crypto_hash_type = string_to_crypto_hash_type(crypto_hash_type);
	instance->crypto_compat_type = string_to_crypto_compat_type(crypto_compat_type);

	instance->crypto_header_size = crypto_sec_header_size(crypto_cipher_type, crypto_hash_type);

	instance->log_printf_func = log_printf_func;
	instance->log_level_security = log_level_security;
	instance->log_level_notice = log_level_notice;
	instance->log_level_error = log_level_error;
	instance->log_subsys_id = log_subsys_id;

	if (init_nss(instance, crypto_cipher_type, crypto_hash_type, crypto_compat_type) < 0) {
		free(instance);
		return(NULL);
	}

	return (instance);
}