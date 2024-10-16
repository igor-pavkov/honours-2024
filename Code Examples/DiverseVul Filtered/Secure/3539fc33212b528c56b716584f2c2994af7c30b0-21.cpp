int octetStringFilter(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	void * assertedValue,
	BerVarray *keysp,
	void *ctx )
{
	BerVarray keys;
	HASH_CONTEXT HASHcontext;
	unsigned char HASHdigest[HASH_BYTES];
	struct berval *value = (struct berval *) assertedValue;
	struct berval digest;
	digest.bv_val = (char *)HASHdigest;
	digest.bv_len = HASH_LEN;

	keys = slap_sl_malloc( sizeof( struct berval ) * 2, ctx );

	hashPreset( &HASHcontext, prefix, 0, syntax, mr );
	hashIter( &HASHcontext, HASHdigest,
		(unsigned char *)value->bv_val, value->bv_len );

	ber_dupbv_x( keys, &digest, ctx );
	BER_BVZERO( &keys[1] );

	*keysp = keys;

	return LDAP_SUCCESS;
}