static int eap_pwd_kdf(uint8_t *key, int keylen, char const *label, int labellen, uint8_t *result, int resultbitlen)
{
	HMAC_CTX *hctx = NULL;
	uint8_t digest[SHA256_DIGEST_LENGTH];
	uint16_t i, ctr, L;
	int resultbytelen, len = 0;
	unsigned int mdlen = SHA256_DIGEST_LENGTH;
	uint8_t mask = 0xff;

	hctx = HMAC_CTX_new();
	if (hctx == NULL) {
		DEBUG("failed allocating HMAC context");
		return -1;
	}
	resultbytelen = (resultbitlen + 7)/8;
	ctr = 0;
	L = htons(resultbitlen);
	while (len < resultbytelen) {
		ctr++; i = htons(ctr);
		HMAC_Init_ex(hctx, key, keylen, EVP_sha256(), NULL);
		if (ctr > 1) {
			HMAC_Update(hctx, digest, mdlen);
		}
		HMAC_Update(hctx, (uint8_t *) &i, sizeof(uint16_t));
		HMAC_Update(hctx, (uint8_t const *)label, labellen);
		HMAC_Update(hctx, (uint8_t *) &L, sizeof(uint16_t));
		HMAC_Final(hctx, digest, &mdlen);
		if ((len + (int) mdlen) > resultbytelen) {
			memcpy(result + len, digest, resultbytelen - len);
		} else {
			memcpy(result + len, digest, mdlen);
		}
		len += mdlen;
	}
	HMAC_CTX_free(hctx);

	/* since we're expanding to a bit length, mask off the excess */
	if (resultbitlen % 8) {
		mask <<= (8 - (resultbitlen % 8));
		result[resultbytelen - 1] &= mask;
	}

	return 0;
}