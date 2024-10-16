static void php_openssl_load_cipher_mode(struct php_openssl_cipher_mode *mode, const EVP_CIPHER *cipher_type) /* {{{ */
{
	switch (EVP_CIPHER_mode(cipher_type)) {
#ifdef EVP_CIPH_GCM_MODE
		case EVP_CIPH_GCM_MODE:
			mode->is_aead = 1;
			mode->is_single_run_aead = 0;
			mode->aead_get_tag_flag = EVP_CTRL_GCM_GET_TAG;
			mode->aead_set_tag_flag = EVP_CTRL_GCM_SET_TAG;
			mode->aead_ivlen_flag = EVP_CTRL_GCM_SET_IVLEN;
			break;
#endif
#ifdef EVP_CIPH_CCM_MODE
		case EVP_CIPH_CCM_MODE:
			mode->is_aead = 1;
			mode->is_single_run_aead = 1;
			mode->aead_get_tag_flag = EVP_CTRL_CCM_GET_TAG;
			mode->aead_set_tag_flag = EVP_CTRL_CCM_SET_TAG;
			mode->aead_ivlen_flag = EVP_CTRL_CCM_SET_IVLEN;
			break;
#endif
		default:
			memset(mode, 0, sizeof(struct php_openssl_cipher_mode));
	}
}