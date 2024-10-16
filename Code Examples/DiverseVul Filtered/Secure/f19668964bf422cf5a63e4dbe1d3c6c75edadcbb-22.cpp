GF_Err ohdr_box_write(GF_Box *s, GF_BitStream *bs)
{
	u16 cid_len, ri_len;
	GF_Err e;
	GF_OMADRMCommonHeaderBox *ptr = (GF_OMADRMCommonHeaderBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->EncryptionMethod);
	gf_bs_write_u8(bs, ptr->PaddingScheme);
	gf_bs_write_u64(bs, ptr->PlaintextLength);

	cid_len = ptr->ContentID ? (u16) strlen(ptr->ContentID) : 0;
	gf_bs_write_u16(bs, cid_len);
	ri_len = ptr->RightsIssuerURL ? (u16) strlen(ptr->RightsIssuerURL) : 0;
	gf_bs_write_u16(bs, ri_len);
	gf_bs_write_u16(bs, ptr->TextualHeadersLen);

	if (cid_len) gf_bs_write_data(bs, ptr->ContentID, (u32) strlen(ptr->ContentID));
	if (ri_len) gf_bs_write_data(bs, ptr->RightsIssuerURL, (u32) strlen(ptr->RightsIssuerURL));
	if (ptr->TextualHeadersLen) gf_bs_write_data(bs, ptr->TextualHeaders, ptr->TextualHeadersLen);

	ISOM_DECREASE_SIZE(ptr, (cid_len+ri_len+ptr->TextualHeadersLen) );
	return GF_OK;
}