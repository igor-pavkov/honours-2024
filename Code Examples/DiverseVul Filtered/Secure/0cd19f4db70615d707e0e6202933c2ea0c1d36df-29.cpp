u32 gf_avc_reformat_sei(u8 *buffer, u32 nal_size, Bool isobmf_rewrite, AVCState *avc)
{
	u32 ptype, psize, hdr, var;
	u32 start;
	GF_BitStream *bs;
	GF_BitStream *bs_dest = NULL;
	u8 nhdr;
	Bool sei_removed = GF_FALSE;
	char store;

	hdr = buffer[0];
	if ((hdr & 0x1F) != GF_AVC_NALU_SEI) return 0;

	if (isobmf_rewrite) bs_dest = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	bs = gf_bs_new(buffer, nal_size, GF_BITSTREAM_READ);
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	nhdr = gf_bs_read_int(bs, 8);
	if (bs_dest) gf_bs_write_int(bs_dest, nhdr, 8);

	/*parse SEI*/
	while (gf_bs_available(bs)) {
		Bool do_copy;
		ptype = 0;
		while (1) {
			u8 v = gf_bs_read_int(bs, 8);
			ptype += v;
			if (v != 0xFF) break;
		}

		psize = 0;
		while (1) {
			u8 v = gf_bs_read_int(bs, 8);
			psize += v;
			if (v != 0xFF) break;
		}

		start = (u32)gf_bs_get_position(bs);

		do_copy = 1;

		if (start + psize >= nal_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[avc-h264] SEI user message type %d size error (%d but %d remain), keeping full SEI untouched\n", ptype, psize, nal_size - start));
			if (bs_dest) gf_bs_del(bs_dest);
			gf_bs_del(bs);
			return nal_size;
		}
		switch (ptype) {
			/*remove SEI messages forbidden in MP4*/
		case 3: /*filler data*/
		case 10: /*sub_seq info*/
		case 11: /*sub_seq_layer char*/
		case 12: /*sub_seq char*/
			do_copy = 0;
			sei_removed = GF_TRUE;
			break;
		case 5: /*user unregistered */
			store = buffer[start + psize];
			buffer[start + psize] = 0;
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CODING, ("[avc-h264] SEI user message %s\n", buffer + start + 16));
			buffer[start + psize] = store;
			break;

		case 6: /*recovery point*/
			avc_parse_recovery_point_sei(bs, avc);
			break;

		case 1: /*pic_timing*/
			avc_parse_pic_timing_sei(bs, avc);
			break;

		case 0: /*buffering period*/
		case 2: /*pan scan rect*/
		case 4: /*user registered ITU t35*/
		case 7: /*def_rec_pic_marking_repetition*/
		case 8: /*spare_pic*/
		case 9: /*scene info*/
		case 13: /*full frame freeze*/
		case 14: /*full frame freeze release*/
		case 15: /*full frame snapshot*/
		case 16: /*progressive refinement segment start*/
		case 17: /*progressive refinement segment end*/
		case 18: /*motion constrained slice group*/
		default: /*add all unknown SEIs*/
			break;
		}

		if (do_copy && bs_dest) {
			var = ptype;
			while (var >= 255) {
				gf_bs_write_int(bs_dest, 0xFF, 8);
				var -= 255;
			}
			gf_bs_write_int(bs_dest, var, 8);

			var = psize;
			while (var >= 255) {
				gf_bs_write_int(bs_dest, 0xFF, 8);
				var -= 255;
			}
			gf_bs_write_int(bs_dest, var, 8);
			gf_bs_seek(bs, start);

			//bs_read_data does not skip EPB, read byte per byte
			var = psize;
			while (var) {
				gf_bs_write_u8(bs_dest, gf_bs_read_u8(bs));
				var--;
			}
		}
		else {
			gf_bs_seek(bs, start);

			//bs_skip_bytes does not skip EPB, skip byte per byte
			while (psize) {
				gf_bs_read_u8(bs);
				psize--;
			}
		}

		if (gf_bs_available(bs) <= 2) {
			var = gf_bs_read_int(bs, 8);
			if (var != 0x80) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[avc-h264] SEI user message has less than 2 bytes remaining but no end of sei found\n"));
			}
			if (bs_dest) gf_bs_write_int(bs_dest, 0x80, 8);
			break;
		}
	}
	gf_bs_del(bs);
	//we cannot compare final size and original size since original may have EPB and final does not yet have them
	if (bs_dest && sei_removed) {
		u8 *dst_no_epb = NULL;
		u32 dst_no_epb_size = 0;
		gf_bs_get_content(bs_dest, &dst_no_epb, &dst_no_epb_size);
		if (dst_no_epb) {
			u32 nb_bytes_add = gf_media_nalu_emulation_bytes_add_count(dst_no_epb, dst_no_epb_size);
			//if result fits into source buffer, reformat
			//otherwise ignore and return source (happens in some fuzzing cases, cf issue 1903)
			if (dst_no_epb_size + nb_bytes_add <= nal_size)
				nal_size = gf_media_nalu_add_emulation_bytes(buffer, dst_no_epb, dst_no_epb_size);

			gf_free(dst_no_epb);
		}
	}
	if (bs_dest) gf_bs_del(bs_dest);
	return nal_size;
}