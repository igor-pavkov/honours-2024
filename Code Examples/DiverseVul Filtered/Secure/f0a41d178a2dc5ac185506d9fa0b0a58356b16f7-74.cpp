u8 *gf_isom_sample_get_subsamples_buffer(GF_ISOFile *movie, u32 track, u32 sampleNumber, u32 *osize)
{
	u8 *data;
	u32 size;
	u32 i, count;
	GF_BitStream *bs = NULL;
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, track);
	if (!trak || !osize) return NULL;
	if (!trak->Media || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->sub_samples) return NULL;

	count = gf_list_count(trak->Media->information->sampleTable->sub_samples);
	for (i=0; i<count; i++) {
		u32 j, sub_count, last_sample = 0;
		GF_SubSampleInformationBox *sub_samples = gf_list_get(trak->Media->information->sampleTable->sub_samples, i);

		sub_count = gf_list_count(sub_samples->Samples);
		for (j=0; j<sub_count; j++) {
			GF_SubSampleInfoEntry *pSamp = (GF_SubSampleInfoEntry *) gf_list_get(sub_samples->Samples, j);
			if (last_sample + pSamp->sample_delta == sampleNumber) {
				u32 scount = gf_list_count(pSamp->SubSamples);
				for (j=0; j<scount; j++) {
					GF_SubSampleEntry *sent = gf_list_get(pSamp->SubSamples, j);
					if (!bs) bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

					gf_bs_write_u32(bs, sub_samples->flags);
					gf_bs_write_u32(bs, sent->subsample_size);
					gf_bs_write_u32(bs, sent->reserved);
					gf_bs_write_u8(bs, sent->subsample_priority);
					gf_bs_write_u8(bs, sent->discardable);
				}
				break;
			}
			last_sample += pSamp->sample_delta;
		}
	}
	if (!bs) return NULL;
	gf_bs_get_content(bs, &data, &size);
	gf_bs_del(bs);
	*osize = size;
	return data;
}