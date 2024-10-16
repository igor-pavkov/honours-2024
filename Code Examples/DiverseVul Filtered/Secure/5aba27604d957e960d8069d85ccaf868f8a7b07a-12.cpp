u64 GetMoovAndMetaSize(GF_ISOFile *movie, GF_List *writers)
{
	u32 i;
	u64 size;

	size = 0;
	if (movie->moov) {
		TrackWriter *writer;
		gf_isom_box_size((GF_Box *)movie->moov);
		size = movie->moov->size;
		if (size > 0xFFFFFFFF) size += 8;

		i=0;
		while ((writer = (TrackWriter*)gf_list_enum(writers, &i))) {
			size -= writer->stbl->ChunkOffset->size;
			size -= writer->stbl->SampleToChunk->size;
			gf_isom_box_size((GF_Box *)writer->stsc);
			gf_isom_box_size(writer->stco);
			size += writer->stsc->size;
			size += writer->stco->size;
		}
	}
	if (movie->meta) {
		u64 msize;
		gf_isom_box_size((GF_Box *)movie->meta);
		msize = movie->meta->size;
		if (msize > 0xFFFFFFFF) msize += 8;
		size += msize;
	}
	return size;
}