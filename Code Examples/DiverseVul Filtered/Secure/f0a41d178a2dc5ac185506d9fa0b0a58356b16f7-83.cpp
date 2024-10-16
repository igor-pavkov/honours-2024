GF_Err gf_isom_get_pssh(GF_ISOFile *file, u32 pssh_index, u8 **pssh_data, u32 *pssh_size)
{
	GF_Err e;
	u32 i=0;
	GF_BitStream *bs;
	u32 count=1;
	GF_Box *pssh;
	while ((pssh = (GF_Box *)gf_list_enum(file->moov->child_boxes, &i))) {
		if (pssh->type != GF_ISOM_BOX_TYPE_PSSH) continue;
		if (count == pssh_index) break;
		count++;
	}
	if (!pssh) return GF_BAD_PARAM;
	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	e = gf_isom_box_write(pssh, bs);
	if (!e) {
		gf_bs_get_content(bs, pssh_data, pssh_size);
	}
	gf_bs_del(bs);
	return e;
}