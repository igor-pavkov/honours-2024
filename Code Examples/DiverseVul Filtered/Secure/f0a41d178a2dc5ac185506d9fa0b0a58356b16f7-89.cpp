u8 gf_isom_get_pl_indication(GF_ISOFile *movie, GF_ISOProfileLevelType PL_Code)
{
	GF_IsomInitialObjectDescriptor *iod;
	if (!movie || !movie->moov) return 0xFF;
	if (!movie->moov->iods || !movie->moov->iods->descriptor) return 0xFF;
	if (movie->moov->iods->descriptor->tag != GF_ODF_ISOM_IOD_TAG) return 0xFF;

	iod = (GF_IsomInitialObjectDescriptor *)movie->moov->iods->descriptor;
	switch (PL_Code) {
	case GF_ISOM_PL_AUDIO:
		return iod->audio_profileAndLevel;
	case GF_ISOM_PL_VISUAL:
		return iod->visual_profileAndLevel;
	case GF_ISOM_PL_GRAPHICS:
		return iod->graphics_profileAndLevel;
	case GF_ISOM_PL_SCENE:
		return iod->scene_profileAndLevel;
	case GF_ISOM_PL_OD:
		return iod->OD_profileAndLevel;
	case GF_ISOM_PL_INLINE:
		return iod->inlineProfileFlag;
	case GF_ISOM_PL_MPEGJ:
	default:
		return 0xFF;
	}
}