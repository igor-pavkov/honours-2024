GF_Err ResetWriters(GF_List *writers)
{
	u32 i;
	TrackWriter *writer;
	i=0;
	while ((writer = (TrackWriter *)gf_list_enum(writers, &i))) {
		writer->isDone = 0;
		writer->chunkDur = 0;
		writer->chunkSize = 0;
		writer->DTSprev = 0;
		writer->sampleNumber = 1;
		gf_isom_box_del((GF_Box *)writer->stsc);
		writer->stsc = (GF_SampleToChunkBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STSC);
		if (!writer->stsc) return GF_OUT_OF_MEM;
		if (writer->stco->type == GF_ISOM_BOX_TYPE_STCO) {
			gf_free(((GF_ChunkOffsetBox *)writer->stco)->offsets);
			((GF_ChunkOffsetBox *)writer->stco)->offsets = NULL;
			((GF_ChunkOffsetBox *)writer->stco)->nb_entries = 0;
			((GF_ChunkOffsetBox *)writer->stco)->alloc_size = 0;
		} else {
			gf_free(((GF_ChunkLargeOffsetBox *)writer->stco)->offsets);
			((GF_ChunkLargeOffsetBox *)writer->stco)->offsets = NULL;
			((GF_ChunkLargeOffsetBox *)writer->stco)->nb_entries = 0;
			((GF_ChunkLargeOffsetBox *)writer->stco)->alloc_size = 0;
		}
	}
	return GF_OK;
}