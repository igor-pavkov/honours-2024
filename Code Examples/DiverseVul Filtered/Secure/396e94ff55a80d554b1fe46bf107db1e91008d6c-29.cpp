static int ike_show_somedata(netdissect_options *ndo,
			     const u_char *cp, const u_char *ep)
{
	/* there is too much data, just show some of it */
	const u_char *end = ep - 20;
	int  elen = 20;
	int   len = ep - cp;
	if(len > 10) {
		len = 10;
	}

	/* really shouldn't happen because of above */
	if(end < cp + len) {
		end = cp+len;
		elen = ep - end;
	}

	ND_PRINT((ndo," data=("));
	if(!rawprint(ndo, (const uint8_t *)(cp), len)) goto trunc;
	ND_PRINT((ndo, "..."));
	if(elen) {
		if(!rawprint(ndo, (const uint8_t *)(end), elen)) goto trunc;
	}
	ND_PRINT((ndo,")"));
	return 1;

trunc:
	return 0;
}