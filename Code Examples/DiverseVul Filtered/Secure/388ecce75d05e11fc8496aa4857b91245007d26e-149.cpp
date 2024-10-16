GF_Err tfxd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MSSTimeExtBox *uuid = (GF_MSSTimeExtBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u8(bs, 1);
	gf_bs_write_u24(bs, 0);
	gf_bs_write_u64(bs, uuid->absolute_time_in_track_timescale);
	gf_bs_write_u64(bs, uuid->fragment_duration_in_track_timescale);

	return GF_OK;
}