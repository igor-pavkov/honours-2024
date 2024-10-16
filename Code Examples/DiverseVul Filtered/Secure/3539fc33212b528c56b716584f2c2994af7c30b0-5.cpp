int slap_hash64( int onoff )
{
	if ( onoff < 0 ) {
		return hashlen == LUTIL_HASH64_BYTES;
	} else if ( onoff ) {
		hashinit = lutil_HASH64Init;
		hashupdate = lutil_HASH64Update;
		hashfinal = lutil_HASH64Final;
		hashlen = LUTIL_HASH64_BYTES;
	} else {
		hashinit = lutil_HASHInit;
		hashupdate = lutil_HASHUpdate;
		hashfinal = lutil_HASHFinal;
		hashlen = LUTIL_HASH_BYTES;
	}
	return 0;
}