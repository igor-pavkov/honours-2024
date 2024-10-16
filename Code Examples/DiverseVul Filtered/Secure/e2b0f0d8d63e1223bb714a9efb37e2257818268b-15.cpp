static const char *metadata_to_str(uint32_t id)
{
	switch (id) {
	case AVRCP_MEDIA_ATTRIBUTE_TITLE:
		return "Title";
	case AVRCP_MEDIA_ATTRIBUTE_ARTIST:
		return "Artist";
	case AVRCP_MEDIA_ATTRIBUTE_ALBUM:
		return "Album";
	case AVRCP_MEDIA_ATTRIBUTE_GENRE:
		return "Genre";
	case AVRCP_MEDIA_ATTRIBUTE_TRACK:
		return "TrackNumber";
	case AVRCP_MEDIA_ATTRIBUTE_N_TRACKS:
		return "NumberOfTracks";
	case AVRCP_MEDIA_ATTRIBUTE_DURATION:
		return "Duration";
	}

	return NULL;
}