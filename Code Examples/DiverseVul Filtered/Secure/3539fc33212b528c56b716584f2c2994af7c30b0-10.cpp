int octetStringIndexer(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	BerVarray values,
	BerVarray *keysp,
	void *ctx )
{
	int i;
	BerVarray keys;
	HASH_CONTEXT HASHcontext;
	unsigned char HASHdigest[HASH_BYTES];
	struct berval digest;
	digest.bv_val = (char *)HASHdigest;
	digest.bv_len = HASH_LEN;

	for( i=0; !BER_BVISNULL( &values[i] ); i++ ) {
		/* just count them */
	}

	/* we should have at least one value at this point */
	assert( i > 0 );

	keys = slap_sl_malloc( sizeof( struct berval ) * (i+1), ctx );

	hashPreset( &HASHcontext, prefix, 0, syntax, mr);
	for( i=0; !BER_BVISNULL( &values[i] ); i++ ) {
		hashIter( &HASHcontext, HASHdigest,
			(unsigned char *)values[i].bv_val, values[i].bv_len );
		ber_dupbv_x( &keys[i], &digest, ctx );
	}

	BER_BVZERO( &keys[i] );

	*keysp = keys;

	return LDAP_SUCCESS;
}