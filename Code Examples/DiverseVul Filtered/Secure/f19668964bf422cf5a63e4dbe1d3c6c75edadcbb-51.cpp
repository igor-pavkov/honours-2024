GF_Err piff_pssh_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_PIFFProtectionSystemHeaderBox *ptr = (GF_PIFFProtectionSystemHeaderBox *) s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->version);
	gf_bs_write_u24(bs, ptr->flags);

	gf_bs_write_data(bs, (char *) ptr->SystemID, 16);
	gf_bs_write_u32(bs, ptr->private_data_size);
	gf_bs_write_data(bs, (char *) ptr->private_data, ptr->private_data_size);
	return GF_OK;
}