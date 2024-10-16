GF_Err pssh_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ProtectionSystemHeaderBox *ptr = (GF_ProtectionSystemHeaderBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_data(bs, (char *) ptr->SystemID, 16);
	if (ptr->version > 0) {
		u32 i;
		gf_bs_write_u32(bs, ptr->KID_count);
		for (i=0; i<ptr->KID_count; i++)
			gf_bs_write_data(bs, (char *) ptr->KIDs[i], 16);
	}
	if (ptr->private_data) {
		gf_bs_write_u32(bs, ptr->private_data_size);
		gf_bs_write_data(bs, (char *) ptr->private_data, ptr->private_data_size);
	} else
		gf_bs_write_u32(bs, 0);
	return GF_OK;
}