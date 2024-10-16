GF_Err gf_isom_write_compressed_box(GF_ISOFile *mov, GF_Box *root_box, u32 repl_type, GF_BitStream *bs, u32 *box_csize)
{
#ifdef GPAC_DISABLE_ZLIB
	return GF_NOT_SUPPORTED;
#else
	GF_Err e;
	GF_BitStream *comp_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	e = gf_isom_box_write(root_box, comp_bs);

	if (!e) {
		u8 *box_data;
		u32 box_size, comp_size;

		if (box_csize)
			*box_csize = (u32) root_box->size;

		gf_bs_get_content(comp_bs, &box_data, &box_size);
		gf_gz_compress_payload_ex(&box_data, box_size, &comp_size, 8, GF_TRUE, NULL);
		if (mov->force_compress || (comp_size + COMP_BOX_COST_BYTES < box_size)) {
			if (bs) {
				gf_bs_write_u32(bs, comp_size+8);
				gf_bs_write_u32(bs, repl_type);
				gf_bs_write_data(bs, box_data, comp_size);
			}
			if (box_csize)
				*box_csize = comp_size + COMP_BOX_COST_BYTES;
		} else if (bs) {
			gf_bs_write_data(bs, box_data, box_size);
		}
		gf_free(box_data);
	}
	gf_bs_del(comp_bs);
	return e;
#endif /*GPAC_DISABLE_ZLIB*/
}