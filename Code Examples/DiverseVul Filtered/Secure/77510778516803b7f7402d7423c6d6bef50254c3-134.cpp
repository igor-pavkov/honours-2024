GF_Err dac3_box_size(GF_Box *s)
{
	GF_AC3ConfigBox *ptr = (GF_AC3ConfigBox *)s;

	if (ptr->cfg.is_ec3) {
		u32 i;
		s->size += 2;
		for (i=0; i<ptr->cfg.nb_streams; i++) {
			s->size += 3;
			if (ptr->cfg.streams[i].nb_dep_sub)
				s->size += 1;
		}
	} else {
		s->size += 3;
	}
	return GF_OK;
}