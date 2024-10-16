GF_Err gf_vp9_parse_superframe(GF_BitStream *bs, u64 ivf_frame_size, u32 *num_frames_in_superframe, u32 frame_sizes[VP9_MAX_FRAMES_IN_SUPERFRAME], u32 *superframe_index_size)
{
	u32 byte, bytes_per_framesize;
	u64 pos = gf_bs_get_position(bs), i = 0;
	GF_Err e;

	assert(bs && num_frames_in_superframe);

	/*initialize like there is no superframe*/
	memset(frame_sizes, 0, VP9_MAX_FRAMES_IN_SUPERFRAME * sizeof(frame_sizes[0]));
	*num_frames_in_superframe = 1;
	frame_sizes[0] = (u32)ivf_frame_size;
	*superframe_index_size = 0;

	e = gf_bs_seek(bs, pos + ivf_frame_size - 1);
	if (e) return e;

	gf_bs_mark_overflow(bs, GF_TRUE);

	byte = gf_bs_read_u8(bs);
	if ((byte & 0xe0) != 0xc0)
		goto exit; /*no superframe*/

	bytes_per_framesize = 1 + ((byte & 0x18) >> 3);
	*num_frames_in_superframe = (u32)(1 + (byte & 0x7));

	/*superframe_index()*/
	*superframe_index_size = 2 + bytes_per_framesize * *num_frames_in_superframe;
	gf_bs_seek(bs, pos + ivf_frame_size - *superframe_index_size);
	byte = gf_bs_read_u8(bs);
	if ((byte & 0xe0) != 0xc0) {
		e = GF_NON_COMPLIANT_BITSTREAM;
		goto exit; /*no superframe*/
	}
	frame_sizes[0] = 0;
	for (i = 0; i < *num_frames_in_superframe; ++i) {
		gf_bs_read_data(bs, (char*)(frame_sizes + i), bytes_per_framesize);
	}

exit:
	gf_bs_seek(bs, pos);

	if (gf_bs_is_overflow(bs)) e = GF_NON_COMPLIANT_BITSTREAM;

	return e;
}