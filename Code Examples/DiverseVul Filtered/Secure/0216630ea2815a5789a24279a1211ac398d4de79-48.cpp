static int php_openssl_pem_password_cb(char *buf, int size, int rwflag, void *userdata)
{
	struct php_openssl_pem_password *password = userdata;

	if (password == NULL || password->key == NULL) {
		return -1;
	}

	size = (password->len > size) ? size : password->len;
	memcpy(buf, password->key, size);

	return size;
}