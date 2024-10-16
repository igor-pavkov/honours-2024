GF_Err ohdr_box_read(GF_Box *s, GF_BitStream *bs)
{
	u16 cid_len, ri_len;
	GF_OMADRMCommonHeaderBox *ptr = (GF_OMADRMCommonHeaderBox*)s;

	ISOM_DECREASE_SIZE(ptr, (1+1+8+2+2+2) );
	ptr->EncryptionMethod = gf_bs_read_u8(bs);
	ptr->PaddingScheme = gf_bs_read_u8(bs);
	ptr->PlaintextLength = gf_bs_read_u64(bs);
	cid_len = gf_bs_read_u16(bs);
	ri_len = gf_bs_read_u16(bs);
	ptr->TextualHeadersLen = gf_bs_read_u16(bs);

	if (ptr->size<cid_len+ri_len+ptr->TextualHeadersLen) return GF_ISOM_INVALID_FILE;

	if (cid_len) {
		ptr->ContentID = (char *)gf_malloc(sizeof(char)*(cid_len+1));
		if (!ptr->ContentID) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->ContentID, cid_len);
		ptr->ContentID[cid_len]=0;
	}

	if (ri_len) {
		ptr->RightsIssuerURL = (char *)gf_malloc(sizeof(char)*(ri_len+1));
		if (!ptr->RightsIssuerURL) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->RightsIssuerURL, ri_len);
		ptr->RightsIssuerURL[ri_len]=0;
	}

	if (ptr->TextualHeadersLen) {
		ptr->TextualHeaders = (char *)gf_malloc(sizeof(char)*(ptr->TextualHeadersLen+1));
		if (!ptr->TextualHeaders) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->TextualHeaders, ptr->TextualHeadersLen);
		ptr->TextualHeaders[ptr->TextualHeadersLen] = 0;
	}

	ISOM_DECREASE_SIZE(ptr, (cid_len+ri_len+ptr->TextualHeadersLen) );

	return gf_isom_box_array_read(s, bs);
}