Bool gf_isom_is_identical_sgpd(void *ptr1, void *ptr2, u32 grouping_type)
{
	Bool res = GF_FALSE;
#ifndef GPAC_DISABLE_ISOM_WRITE
	GF_BitStream *bs1, *bs2;
	u8 *buf1, *buf2;
	u32 len1, len2;

	if (!ptr1 || !ptr2)
		return GF_FALSE;

	bs1 = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	if (grouping_type) {
		sgpd_write_entry(grouping_type, ptr1, bs1);
	} else {
		gf_isom_box_size((GF_Box *)ptr1);
		gf_isom_box_write((GF_Box *)ptr1, bs1);
	}
	gf_bs_get_content(bs1, &buf1, &len1);
	gf_bs_del(bs1);

	bs2 = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	if (grouping_type) {
		sgpd_write_entry(grouping_type, ptr2, bs2);
	} else {
		gf_isom_box_write((GF_Box *)ptr2, bs2);
	}
	gf_bs_get_content(bs2, &buf2, &len2);
	gf_bs_del(bs2);


	if ((len1==len2) && !memcmp(buf1, buf2, len1))
		res = GF_TRUE;

	gf_free(buf1);
	gf_free(buf2);
#endif
	return res;
}