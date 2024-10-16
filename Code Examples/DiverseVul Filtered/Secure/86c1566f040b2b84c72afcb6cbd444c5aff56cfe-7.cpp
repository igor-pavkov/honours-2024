Bool IsHintTrack(GF_TrackBox *trak)
{
	if (trak->Media->handler->handlerType != GF_ISOM_MEDIA_HINT) return GF_FALSE;
	//QT doesn't specify any InfoHeader on HintTracks
	if (trak->Media->information->InfoHeader
	        && (trak->Media->information->InfoHeader->type != GF_ISOM_BOX_TYPE_HMHD)
	        && (trak->Media->information->InfoHeader->type != GF_ISOM_BOX_TYPE_NMHD)
		)
		return GF_FALSE;

	return GF_TRUE;
}