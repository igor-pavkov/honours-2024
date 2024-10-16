GF_Err fpar_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	FilePartitionBox *ptr = (FilePartitionBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_int(bs, ptr->itemID, ptr->version ? 32 : 16);
	gf_bs_write_u16(bs, ptr->packet_payload_size);
	gf_bs_write_u8(bs, 0);
	gf_bs_write_u8(bs, ptr->FEC_encoding_ID);
	gf_bs_write_u16(bs, ptr->FEC_instance_ID);
	gf_bs_write_u16(bs, ptr->max_source_block_length);
	gf_bs_write_u16(bs, ptr->encoding_symbol_length);
	gf_bs_write_u16(bs, ptr->max_number_of_encoding_symbols);
	if (ptr->scheme_specific_info) {
		gf_bs_write_data(bs, ptr->scheme_specific_info, (u32)strlen(ptr->scheme_specific_info) );
	}
	//null terminated string
	gf_bs_write_u8(bs, 0);

	gf_bs_write_int(bs, ptr->nb_entries, ptr->version ? 32 : 16);

	for (i=0;i < ptr->nb_entries; i++) {
		gf_bs_write_u16(bs, ptr->entries[i].block_count);
		gf_bs_write_u32(bs, ptr->entries[i].block_size);
	}
	return GF_OK;
}