int generalizedTimeFilter(
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
	char tmp[5];
	BerValue bvtmp; /* 40 bit index */
	BerValue *value = (BerValue *) assertedValue;
	struct lutil_tm tm;
	struct lutil_timet tt;
	
	bvtmp.bv_len = sizeof(tmp);
	bvtmp.bv_val = tmp;
	/* GeneralizedTime YYYYmmddHH[MM[SS]][(./,)d...](Z|(+/-)HH[MM]) */
	/* Use 40 bits of time for key */
	if ( value->bv_val && value->bv_len >= 10 &&
		lutil_parsetime( value->bv_val, &tm ) == 0 ) {

		lutil_tm2gtime( &tm, &tt );
		tmp[0] = tt.tt_gsec & 0xff;
		tmp[4] = tt.tt_sec & 0xff;
		tt.tt_sec >>= 8;
		tmp[3] = tt.tt_sec & 0xff;
		tt.tt_sec >>= 8;
		tmp[2] = tt.tt_sec & 0xff;
		tt.tt_sec >>= 8;
		tmp[1] = tt.tt_sec & 0xff;

		keys = slap_sl_malloc( sizeof( struct berval ) * 2, ctx );
		ber_dupbv_x(keys, &bvtmp, ctx );
		keys[1].bv_val = NULL;
		keys[1].bv_len = 0;
	} else {
		keys = NULL;
	}

	*keysp = keys;

	return LDAP_SUCCESS;
}