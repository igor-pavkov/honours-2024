int AVI_close(avi_t *AVI)
{
	int ret;
	u32 j;

	/* If the file was open for writing, the header and index still have
	   to be written */

	if(AVI->mode == AVI_MODE_WRITE)
		ret = avi_close_output_file(AVI);
	else
		ret = 0;

	/* Even if there happened an error, we first clean up */

	gf_fclose(AVI->fdes);
	if(AVI->idx) gf_free(AVI->idx);
	if(AVI->video_index) gf_free(AVI->video_index);
	if(AVI->video_superindex) {
		if(AVI->video_superindex->aIndex) gf_free(AVI->video_superindex->aIndex);
		if (AVI->video_superindex->stdindex) {
			for (j=0; j < NR_IXNN_CHUNKS; j++) {
				if (AVI->video_superindex->stdindex[j]->aIndex)
					gf_free(AVI->video_superindex->stdindex[j]->aIndex);
				gf_free(AVI->video_superindex->stdindex[j]);
			}
			gf_free(AVI->video_superindex->stdindex);
		}
		gf_free(AVI->video_superindex);
	}

	for (j=0; j<AVI->anum; j++)
	{
		if(AVI->track[j].audio_index) gf_free(AVI->track[j].audio_index);
		if(AVI->track[j].audio_superindex) {
			avisuperindex_chunk *asi = AVI->track[j].audio_superindex;
			if (asi->aIndex) gf_free(asi->aIndex);

			if (asi->stdindex) {
				for (j=0; j < NR_IXNN_CHUNKS; j++) {
					if (asi->stdindex[j]->aIndex)
						gf_free(asi->stdindex[j]->aIndex);
					gf_free(asi->stdindex[j]);
				}
				gf_free(asi->stdindex);
			}
			gf_free(asi);
		}
	}

	if (AVI->bitmap_info_header)
		gf_free(AVI->bitmap_info_header);
	for (j = 0; j < AVI->anum; j++)
		if (AVI->wave_format_ex[j])
			gf_free(AVI->wave_format_ex[j]);
	if (AVI->extradata)
		gf_free(AVI->extradata);

	gf_free(AVI);
	return ret;
}