Bool Track_IsMPEG4Stream(u32 HandlerType)
{
	switch (HandlerType) {
	case GF_ISOM_MEDIA_VISUAL:
    case GF_ISOM_MEDIA_AUXV:
    case GF_ISOM_MEDIA_PICT:
	case GF_ISOM_MEDIA_AUDIO:
	case GF_ISOM_MEDIA_SUBPIC:
	case GF_ISOM_MEDIA_OD:
	case GF_ISOM_MEDIA_OCR:
	case GF_ISOM_MEDIA_SCENE:
	case GF_ISOM_MEDIA_MPEG7:
	case GF_ISOM_MEDIA_OCI:
	case GF_ISOM_MEDIA_IPMP:
	case GF_ISOM_MEDIA_MPEGJ:
	case GF_ISOM_MEDIA_ESM:
		return 1;
	/*Timedtext is NOT an MPEG-4 stream*/
	default:
		/*consider xxsm as MPEG-4 handlers*/
		if ( (((HandlerType>>8) & 0xFF)== 's') && ((HandlerType& 0xFF)== 'm'))
			return 1;
		return 0;
	}
}