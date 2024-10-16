void update_writer_constant_dur(GF_ISOFile *movie, TrackWriter *tkw, GF_StscEntry *stsc_ent, u32 *nb_samp, u32 *samp_size, Bool is_flat)
{
	u64 chunk_dur;
	u32 nb_in_run;
	u32 samp_idx_in_chunk, nb_samp_left_in_src_chunk;
	if (!tkw->constant_dur) return;

	samp_idx_in_chunk = tkw->sampleNumber - tkw->stbl->SampleToChunk->firstSampleInCurrentChunk;
	nb_samp_left_in_src_chunk = stsc_ent->samplesPerChunk - samp_idx_in_chunk;

	if (nb_samp_left_in_src_chunk<=1) return;

	if (is_flat) {
		nb_in_run = nb_samp_left_in_src_chunk;
	} else {

		chunk_dur = movie->interleavingTime * tkw->timeScale;
		if (movie->moov && movie->moov->mvhd && movie->moov->mvhd->timeScale)
			chunk_dur /= movie->moov->mvhd->timeScale;

		chunk_dur -= tkw->chunkDur;

		if (chunk_dur <= tkw->chunkDur) return;
		chunk_dur -= tkw->constant_dur;

		nb_in_run = (u32) (chunk_dur / tkw->constant_dur);

		if (nb_in_run > nb_samp_left_in_src_chunk) {
			nb_in_run = nb_samp_left_in_src_chunk;
		}
	}
	if (tkw->sampleNumber + nb_in_run >= tkw->stbl->SampleSize->sampleCount) {
		nb_in_run = tkw->stbl->SampleSize->sampleCount - tkw->sampleNumber;
	}

	chunk_dur = nb_in_run * tkw->constant_dur;

	tkw->chunkDur += (u32) chunk_dur - tkw->constant_dur; //because tkw->chunkDur already include duration of first sample of chunk
	tkw->DTSprev += chunk_dur - tkw->constant_dur; //because nb_samp += nb_in_run-1

	*nb_samp = nb_in_run;
	*samp_size = nb_in_run * tkw->constant_size;
}