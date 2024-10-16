u32 gf_isom_get_fragments_count(GF_ISOFile *movie, Bool segments_only)
{
	u32 i=0;
	u32 nb_frags = 0;
	GF_Box *b;
	while ((b=(GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
		if (segments_only) {
			if (b->type==GF_ISOM_BOX_TYPE_SIDX)
				nb_frags++;
		} else {
			if (b->type==GF_ISOM_BOX_TYPE_MOOF)
				nb_frags++;
		}
	}
	return nb_frags;
}