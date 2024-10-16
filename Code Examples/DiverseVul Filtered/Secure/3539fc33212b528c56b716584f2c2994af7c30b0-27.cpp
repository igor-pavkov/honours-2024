int generalizedTimeIndexer(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	BerVarray values,
	BerVarray *keysp,
	void *ctx )
{
	int i, j;
	BerVarray keys;
	char tmp[5];
	BerValue bvtmp; /* 40 bit index */
	struct lutil_tm tm;
	struct lutil_timet tt;

	bvtmp.bv_len = sizeof(tmp);
	bvtmp.bv_val = tmp;
	for( i=0; values[i].bv_val != NULL; i++ ) {
		/* just count them */
	}

	/* we should have at least one value at this point */
	assert( i > 0 );

	keys = slap_sl_malloc( sizeof( struct berval ) * (i+1), ctx );

	/* GeneralizedTime YYYYmmddHH[MM[SS]][(./,)d...](Z|(+/-)HH[MM]) */
	for( i=0, j=0; values[i].bv_val != NULL; i++ ) {
		assert(values[i].bv_val != NULL && values[i].bv_len >= 10);
		/* Use 40 bits of time for key */
		if ( lutil_parsetime( values[i].bv_val, &tm ) == 0 ) {
			lutil_tm2gtime( &tm, &tt );
			tmp[0] = tt.tt_gsec & 0xff;
			tmp[4] = tt.tt_sec & 0xff;
			tt.tt_sec >>= 8;
			tmp[3] = tt.tt_sec & 0xff;
			tt.tt_sec >>= 8;
			tmp[2] = tt.tt_sec & 0xff;
			tt.tt_sec >>= 8;
			tmp[1] = tt.tt_sec & 0xff;
			
			ber_dupbv_x(&keys[j++], &bvtmp, ctx );
		}
	}

	keys[j].bv_val = NULL;
	keys[j].bv_len = 0;

	*keysp = keys;

	return LDAP_SUCCESS;
}