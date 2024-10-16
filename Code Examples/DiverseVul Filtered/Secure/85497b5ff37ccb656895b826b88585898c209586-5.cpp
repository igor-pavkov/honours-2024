static void H_Final(HMAC_CTX *ctx, uint8_t *digest)
{
	unsigned int mdlen = SHA256_DIGEST_LENGTH;

	HMAC_Final(ctx, digest, &mdlen);
}