static void H_Init(HMAC_CTX *ctx)
{
	uint8_t allzero[SHA256_DIGEST_LENGTH];

	memset(allzero, 0, SHA256_DIGEST_LENGTH);

	HMAC_Init_ex(ctx, allzero, SHA256_DIGEST_LENGTH, EVP_sha256(), NULL);
}