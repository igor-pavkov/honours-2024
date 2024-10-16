Bool gf_isom_is_single_av(GF_ISOFile *file)
{
	u32 count, i, nb_any, nb_a, nb_v, nb_auxv, nb_pict, nb_scene, nb_od, nb_text;
	nb_auxv = nb_pict = nb_a = nb_v = nb_any = nb_scene = nb_od = nb_text = 0;

	if (!file->moov) return GF_FALSE;
	count = gf_isom_get_track_count(file);
	for (i=0; i<count; i++) {
		u32 mtype = gf_isom_get_media_type(file, i+1);
		switch (mtype) {
		case GF_ISOM_MEDIA_SCENE:
			if (gf_isom_get_sample_count(file, i+1)>1) nb_any++;
			else nb_scene++;
			break;
		case GF_ISOM_MEDIA_OD:
			if (gf_isom_get_sample_count(file, i+1)>1) nb_any++;
			else nb_od++;
			break;
		case GF_ISOM_MEDIA_TEXT:
		case GF_ISOM_MEDIA_SUBT:
			nb_text++;
			break;
		case GF_ISOM_MEDIA_AUDIO:
			nb_a++;
			break;
        case GF_ISOM_MEDIA_AUXV:
            /*discard file with images*/
            if (gf_isom_get_sample_count(file, i+1)==1) nb_any++;
            else nb_auxv++;
            break;
        case GF_ISOM_MEDIA_PICT:
            /*discard file with images*/
            if (gf_isom_get_sample_count(file, i+1)==1) nb_any++;
            else nb_pict++;
            break;
		case GF_ISOM_MEDIA_VISUAL:
			/*discard file with images*/
			if (gf_isom_get_sample_count(file, i+1)==1) nb_any++;
			else nb_v++;
			break;
		default:
			nb_any++;
			break;
		}
	}
	if (nb_any) return GF_FALSE;
	if ((nb_scene<=1) && (nb_od<=1) && (nb_a<=1) && (nb_v+nb_pict+nb_auxv<=1) && (nb_text<=1) ) return GF_TRUE;
	return GF_FALSE;
}