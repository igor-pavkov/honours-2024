static void do_ipod_conv()
{
	u32 i, ipod_major_brand = 0;
	M4_LOG(GF_LOG_INFO, ("Setting up iTunes/iPod file\n"));

	for (i=0; i<gf_isom_get_track_count(file); i++) {
		u32 mType = gf_isom_get_media_type(file, i+1);
		switch (mType) {
		case GF_ISOM_MEDIA_VISUAL:
		case GF_ISOM_MEDIA_AUXV:
		case GF_ISOM_MEDIA_PICT:
			ipod_major_brand = GF_ISOM_BRAND_M4V;
			gf_isom_set_ipod_compatible(file, i+1);
			break;
		case GF_ISOM_MEDIA_AUDIO:
			if (!ipod_major_brand) ipod_major_brand = GF_ISOM_BRAND_M4A;
			else gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_M4A, GF_TRUE);
			break;
		case GF_ISOM_MEDIA_TEXT:
			/*this is a text track track*/
			if (gf_isom_get_media_subtype(file, i+1, 1) == GF_ISOM_SUBTYPE_TX3G) {
				Bool is_chap = 0;
				u32 j;
				for (j=0; j<gf_isom_get_track_count(file); j++) {
					s32 count = gf_isom_get_reference_count(file, j+1, GF_ISOM_REF_CHAP);
					if (count>0) {
						u32 tk, k;
						for (k=0; k<(u32) count; k++) {
							gf_isom_get_reference(file, j+1, GF_ISOM_REF_CHAP, k+1, &tk);
							if (tk==i+1) {
								is_chap = 1;
								break;
							}
						}
						if (is_chap) break;
					}
					if (is_chap) break;
				}
				/*this is a subtitle track*/
				if (!is_chap)
					gf_isom_set_media_type(file, i+1, GF_ISOM_MEDIA_SUBT);
			}
			break;
		}
	}
	gf_isom_set_brand_info(file, ipod_major_brand, 1);
	gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_MP42, GF_TRUE);
	do_save = GF_TRUE;
}