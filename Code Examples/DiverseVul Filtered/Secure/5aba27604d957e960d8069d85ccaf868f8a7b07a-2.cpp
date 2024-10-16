static GF_Err WriteMoovAndMeta(GF_ISOFile *movie, GF_List *writers, GF_BitStream *bs)
{
	u32 i;
	TrackWriter *writer;
	GF_Err e;
	GF_Box *stco;
	GF_SampleToChunkBox *stsc;

	if (movie->meta) {
		//write the moov box...
		e = gf_isom_box_size((GF_Box *)movie->meta);
		if (e) return e;
		e = gf_isom_box_write((GF_Box *)movie->meta, bs);
		if (e) return e;
	}

	if (movie->moov) {
		Bool prevent_dispatch = GF_FALSE;
		//switch all our tables
		i=0;
		while ((writer = (TrackWriter*)gf_list_enum(writers, &i))) {
			//don't delete them !!!
			stsc = writer->stbl->SampleToChunk;
			stco = writer->stbl->ChunkOffset;
			s32 stsc_pos = gf_list_del_item(writer->stbl->child_boxes, stsc);
			s32 stco_pos = gf_list_del_item(writer->stbl->child_boxes, stco);
			writer->stbl->SampleToChunk = writer->stsc;
			writer->stbl->ChunkOffset = writer->stco;
			gf_list_insert(writer->stbl->child_boxes, writer->stsc, stsc_pos);
			gf_list_insert(writer->stbl->child_boxes, writer->stco, stco_pos);
			writer->stco = stco;
			writer->stsc = stsc;
			if (writer->prevent_dispatch)
				prevent_dispatch = GF_TRUE;
		}
		if (prevent_dispatch) {
			gf_bs_prevent_dispatch(bs, GF_TRUE);
		}
		//write the moov box...
		e = gf_isom_box_size((GF_Box *)movie->moov);
		if (e) return e;

		if ((movie->compress_mode==GF_ISO_COMP_ALL) || (movie->compress_mode==GF_ISO_COMP_MOOV)) {
			e = gf_isom_write_compressed_box(movie, (GF_Box *) movie->moov, GF_4CC('!', 'm', 'o', 'v'), bs, NULL);
		} else {
			e = gf_isom_box_write((GF_Box *)movie->moov, bs);
		}

		if (prevent_dispatch) {
			gf_bs_prevent_dispatch(bs, GF_FALSE);
		}

		//and re-switch our table. We have to do it that way because it is
		//needed when the moov is written first
		i=0;
		while ((writer = (TrackWriter*)gf_list_enum(writers, &i))) {
			//don't delete them !!!
			stsc = writer->stsc;
			stco = writer->stco;
			writer->stsc = writer->stbl->SampleToChunk;
			writer->stco = writer->stbl->ChunkOffset;
			s32 stsc_pos = gf_list_del_item(writer->stbl->child_boxes, writer->stsc);
			s32 stco_pos = gf_list_del_item(writer->stbl->child_boxes, writer->stco);

			writer->stbl->SampleToChunk = stsc;
			writer->stbl->ChunkOffset = stco;
			gf_list_insert(writer->stbl->child_boxes, stsc, stsc_pos);
			gf_list_insert(writer->stbl->child_boxes, stco, stco_pos);
		}
		if (e) return e;
	}
	return GF_OK;
}