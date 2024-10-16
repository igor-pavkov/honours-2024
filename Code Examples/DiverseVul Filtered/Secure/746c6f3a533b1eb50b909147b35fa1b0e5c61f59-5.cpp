const EVP_CIPHER *EVP_aes_256_cbc_hmac_sha1(void)
	{
	return(OPENSSL_ia32cap_P[1]&AESNI_CAPABLE?
		&aesni_256_cbc_hmac_sha1_cipher:NULL);
	}