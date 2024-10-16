static GF_Err gf_dump_to_ogg(GF_MediaExporter *dumper, char *szName, u32 track)
{
#ifdef GPAC_DISABLE_OGG
	return GF_NOT_SUPPORTED;
#else
	FILE *out;
	ogg_stream_state os;
	ogg_packet op;
	ogg_page og;
	u32 count, i, di, theora_kgs, nb_i, nb_p;
	Bool flush_first = GF_TRUE;
	GF_BitStream *bs;
	GF_ISOSample *samp;
	GF_ESD *esd = gf_isom_get_esd(dumper->file, track, 1);


	memset(&os, 0, sizeof(ogg_stream_state));
	memset(&og, 0, sizeof(ogg_page));
	memset(&op, 0, sizeof(ogg_packet));

	if (gf_sys_is_test_mode()) {
		ogg_stream_init(&os, 1);
	} else {
		gf_rand_init(GF_TRUE);
		ogg_stream_init(&os, gf_rand());
	}

	out = szName ? gf_fopen(szName, "wb") : stdout;
	if (!out) return gf_export_message(dumper, GF_IO_ERR, "Error opening %s for writing - check disk access & permissions", szName);

	theora_kgs = 0;
	bs = gf_bs_new(esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, GF_BITSTREAM_READ);
	if (esd->decoderConfig->objectTypeIndication==GF_CODECID_OPUS) {
		GF_BitStream *bs_out;
		GF_OpusSpecificBox *dops = (GF_OpusSpecificBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_DOPS);
		dops->size = gf_bs_read_u32(bs);
		gf_bs_read_u32(bs);
		gf_isom_box_read((GF_Box *)dops, bs);
		bs_out = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
		gf_bs_write_data(bs_out, "OpusHead", 8);
		gf_bs_write_u8(bs_out, 1);//version
		gf_bs_write_u8(bs_out, dops->OutputChannelCount);
		gf_bs_write_u16_le(bs_out, dops->PreSkip);
		gf_bs_write_u32_le(bs_out, dops->InputSampleRate);
		gf_bs_write_u16_le(bs_out, dops->OutputGain);
		gf_bs_write_u8(bs_out, dops->ChannelMappingFamily);
		if (dops->ChannelMappingFamily) {
			gf_bs_write_u8(bs_out, dops->StreamCount);
			gf_bs_write_u8(bs_out, dops->CoupledCount);
			gf_bs_write_data(bs, (char *) dops->ChannelMapping, dops->OutputChannelCount);
		}
		gf_isom_box_del((GF_Box*)dops);

		gf_bs_get_content(bs_out, &op.packet, &op.bytes);
		gf_bs_del(bs_out);
		ogg_stream_packetin(&os, &op);
		gf_free(op.packet);
		op.packetno ++;

	} else {
		while (gf_bs_available(bs)) {
			op.bytes = gf_bs_read_u16(bs);
			op.packet = (unsigned char*)gf_malloc(sizeof(char) * op.bytes);
			gf_bs_read_data(bs, (char*)op.packet, op.bytes);
			ogg_stream_packetin(&os, &op);

			if (flush_first) {
				ogg_stream_pageout(&os, &og);
				gf_fwrite(og.header, og.header_len, out);
				gf_fwrite(og.body, og.body_len, out);
				flush_first = 0;

				if (esd->decoderConfig->objectTypeIndication==GF_CODECID_THEORA) {
					u32 kff;
					GF_BitStream *vbs = gf_bs_new((char*)op.packet, op.bytes, GF_BITSTREAM_READ);
					gf_bs_skip_bytes(vbs, 40);
					gf_bs_read_int(vbs, 6); /* quality */
					kff = 1 << gf_bs_read_int(vbs, 5);
					gf_bs_del(vbs);

					theora_kgs = 0;
					kff--;
					while (kff) {
						theora_kgs ++;
						kff >>= 1;
					}
				}
			}
			gf_free(op.packet);
			op.packetno ++;
		}
	}
	gf_bs_del(bs);
	gf_odf_desc_del((GF_Descriptor *)esd);

	while (ogg_stream_pageout(&os, &og)>0) {
		gf_fwrite(og.header, og.header_len, out);
		gf_fwrite(og.body, og.body_len, out);
	}

	op.granulepos = -1;

	count = gf_isom_get_sample_count(dumper->file, track);

	nb_i = nb_p = 0;
	samp = gf_isom_get_sample(dumper->file, track, 1, &di);
	for (i=0; i<count; i++) {
		GF_ISOSample *next_samp = gf_isom_get_sample(dumper->file, track, i+2, &di);
		if (!samp) break;
		op.bytes = samp->dataLength;
		op.packet = (unsigned char*)samp->data;
		op.packetno ++;

		if (theora_kgs) {
			if (samp->IsRAP) {
				if (i) nb_i+=nb_p+1;
				nb_p = 0;
			} else {
				nb_p++;
			}
			op.granulepos = nb_i;
			op.granulepos <<= theora_kgs;
			op.granulepos |= nb_p;
		} else {
			if (next_samp) op.granulepos = next_samp->DTS;
		}
		if (!next_samp) op.e_o_s = 1;

		ogg_stream_packetin(&os, &op);

		gf_isom_sample_del(&samp);
		samp = next_samp;
		next_samp = NULL;
		gf_set_progress("OGG Export", i+1, count);
		if (dumper->flags & GF_EXPORT_DO_ABORT) break;

		while (ogg_stream_pageout(&os, &og)>0) {
			gf_fwrite(og.header, og.header_len, out);
			gf_fwrite(og.body, og.body_len, out);
		}
	}
	if (samp) gf_isom_sample_del(&samp);

	while (ogg_stream_flush(&os, &og)>0) {
		gf_fwrite(og.header, og.header_len, out);
		gf_fwrite(og.body, og.body_len, out);
	}
	ogg_stream_clear(&os);
	if (szName) gf_fclose(out);
	return GF_OK;
#endif
}