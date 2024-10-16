Bool gf_isom_is_video_handler_type(u32 mtype)
{
	switch (mtype) {
	case GF_ISOM_MEDIA_VISUAL:
	case GF_ISOM_MEDIA_AUXV:
	case GF_ISOM_MEDIA_PICT:
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
}