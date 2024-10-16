static GF_Err gf_isom_get_3gpp_audio_esd(GF_SampleTableBox *stbl, u32 type, GF_GenericAudioSampleEntryBox *entry, GF_ESD **out_esd)
{
	(*out_esd) = gf_odf_desc_esd_new(2);
	(*out_esd)->decoderConfig->streamType = GF_STREAM_AUDIO;
	/*official mapping to MPEG-4*/
	switch (type) {
	case GF_ISOM_SUBTYPE_3GP_EVRC:
		(*out_esd)->decoderConfig->objectTypeIndication = GF_CODECID_EVRC;
		return GF_OK;
	case GF_ISOM_SUBTYPE_3GP_QCELP:
	{
		u32 block_size, sample_rate, sample_size, i;
		GF_SttsEntry *ent;
		GF_BitStream *bs;
		char szName[80];
		/*only map CBR*/
		sample_size = stbl->SampleSize->sampleSize;
		(*out_esd)->decoderConfig->objectTypeIndication = GF_CODECID_QCELP;
		bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
		gf_bs_write_data(bs, "QLCMfmt ", 8);
		gf_bs_write_u32_le(bs, 150);/*fmt chunk size*/
		gf_bs_write_u8(bs, 1);
		gf_bs_write_u8(bs, 0);
		/*QCELP GUID*/
		gf_bs_write_data(bs, "\x41\x6D\x7F\x5E\x15\xB1\xD0\x11\xBA\x91\x00\x80\x5F\xB4\xB9\x7E", 16);
		gf_bs_write_u16_le(bs, 1);
		memset(szName, 0, 80);
		strcpy(szName, "QCELP-13K(GPAC-emulated)");
		gf_bs_write_data(bs, szName, 80);
		ent = &stbl->TimeToSample->entries[0];
		sample_rate = entry->samplerate_hi;
		block_size = ent ? ent->sampleDelta : 160;
		gf_bs_write_u16_le(bs, 8*sample_size*sample_rate/block_size);
		gf_bs_write_u16_le(bs, sample_size);
		gf_bs_write_u16_le(bs, block_size);
		gf_bs_write_u16_le(bs, sample_rate);
		gf_bs_write_u16_le(bs, entry->bitspersample);
		gf_bs_write_u32_le(bs, sample_size ? 0 : 7);
		/**/
		for (i=0; i<7; i++) {
			static const u32 qcelp_r2s [] = {0, 1, 1, 4, 2, 8, 3, 17, 4, 35, 5, 8, 14, 1};
			if (sample_size) {
				gf_bs_write_u16(bs, 0);
			} else {
				gf_bs_write_u8(bs, qcelp_r2s[2*i+1]);
				gf_bs_write_u8(bs, qcelp_r2s[2*i]);
			}
		}
		gf_bs_write_u16(bs, 0);
		memset(szName, 0, 80);
		gf_bs_write_data(bs, szName, 20);/*reserved*/
		gf_bs_get_content(bs, & (*out_esd)->decoderConfig->decoderSpecificInfo->data, & (*out_esd)->decoderConfig->decoderSpecificInfo->dataLength);
		gf_bs_del(bs);
	}
	return GF_OK;
	case GF_ISOM_SUBTYPE_3GP_SMV:
		(*out_esd)->decoderConfig->objectTypeIndication = GF_CODECID_SMV;
		return GF_OK;
	case GF_ISOM_SUBTYPE_3GP_AMR:
		(*out_esd)->decoderConfig->objectTypeIndication = GF_CODECID_AMR;
		return GF_OK;
	case GF_ISOM_SUBTYPE_3GP_AMR_WB:
		(*out_esd)->decoderConfig->objectTypeIndication = GF_CODECID_AMR_WB;
		return GF_OK;
	default:
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] unsupported sample description type %s\n", gf_4cc_to_str(entry->type)));
		break;
	}
	return GF_OK;
}