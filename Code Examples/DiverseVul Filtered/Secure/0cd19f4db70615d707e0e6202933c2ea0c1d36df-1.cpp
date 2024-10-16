GF_Err gf_media_parse_ivf_frame_header(GF_BitStream *bs, u64 *frame_size, u64 *pts)
{
	if (!frame_size) return GF_BAD_PARAM;
	if (gf_bs_available(bs) < 12)
		return GF_BUFFER_TOO_SMALL;

	*frame_size = gf_bs_read_u32_le(bs);
	if (*frame_size > 256 * 1024 * 1024) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[IVF] Wrong frame size %u\n", *frame_size));
		*frame_size = 0;
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	*pts = gf_bs_read_u64_le(bs);

	return GF_OK;
}