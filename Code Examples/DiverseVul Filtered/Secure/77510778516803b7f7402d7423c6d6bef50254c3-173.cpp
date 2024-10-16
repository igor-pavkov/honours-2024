GF_Err reftype_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TrackReferenceTypeBox *ptr = (GF_TrackReferenceTypeBox *)s;
	ptr->type = ptr->reference_type;
	e = gf_isom_box_write_header(s, bs);
	ptr->type = GF_ISOM_BOX_TYPE_REFT;
	if (e) return e;
	for (i = 0; i < ptr->trackIDCount; i++) {
		gf_bs_write_u32(bs, ptr->trackIDs[i]);
	}
	return GF_OK;
}