void tls_fips_digest_extra(
	const EVP_CIPHER_CTX *cipher_ctx, EVP_MD_CTX *mac_ctx,
	const unsigned char *data, size_t data_len, size_t orig_len)
	{
	size_t block_size, digest_pad, blocks_data, blocks_orig;
	if (EVP_CIPHER_CTX_mode(cipher_ctx) != EVP_CIPH_CBC_MODE)
		return;
	block_size = EVP_MD_CTX_block_size(mac_ctx);
	/* We are in FIPS mode if we get this far so we know we have only SHA*
	 * digests and TLS to deal with.
	 * Minimum digest padding length is 17 for SHA384/SHA512 and 9
	 * otherwise.
	 * Additional header is 13 bytes. To get the number of digest blocks
	 * processed round up the amount of data plus padding to the nearest
	 * block length. Block length is 128 for SHA384/SHA512 and 64 otherwise.
	 * So we have:
	 * blocks = (payload_len + digest_pad + 13 + block_size - 1)/block_size
	 * equivalently:
	 * blocks = (payload_len + digest_pad + 12)/block_size + 1
	 * HMAC adds a constant overhead.
	 * We're ultimately only interested in differences so this becomes
	 * blocks = (payload_len + 29)/128
	 * for SHA384/SHA512 and
	 * blocks = (payload_len + 21)/64
	 * otherwise.
	 */
	digest_pad = block_size == 64 ? 21 : 29;
	blocks_orig = (orig_len + digest_pad)/block_size;
	blocks_data = (data_len + digest_pad)/block_size;
	/* MAC enough blocks to make up the difference between the original
	 * and actual lengths plus one extra block to ensure this is never a
	 * no op. The "data" pointer should always have enough space to
	 * perform this operation as it is large enough for a maximum
	 * length TLS buffer. 
	 */
	EVP_DigestSignUpdate(mac_ctx, data,
				(blocks_orig - blocks_data + 1) * block_size);
	}