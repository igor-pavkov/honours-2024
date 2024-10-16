static void H_Update(HMAC_CTX *ctx, uint8_t const *data, int len)
{
	HMAC_Update(ctx, data, len);
}