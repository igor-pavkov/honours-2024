static uint32_t str_to_metadata(const char *str)
{
	if (strcasecmp(str, "Title") == 0)
		return AVRCP_MEDIA_ATTRIBUTE_TITLE;
	else if (strcasecmp(str, "Artist") == 0)
		return AVRCP_MEDIA_ATTRIBUTE_ARTIST;
	else if (strcasecmp(str, "Album") == 0)
		return AVRCP_MEDIA_ATTRIBUTE_ALBUM;
	else if (strcasecmp(str, "Genre") == 0)
		return AVRCP_MEDIA_ATTRIBUTE_GENRE;
	else if (strcasecmp(str, "TrackNumber") == 0)
		return AVRCP_MEDIA_ATTRIBUTE_TRACK;
	else if (strcasecmp(str, "NumberOfTracks") == 0)
		return AVRCP_MEDIA_ATTRIBUTE_N_TRACKS;
	else if (strcasecmp(str, "Duration") == 0)
		return AVRCP_MEDIA_ATTRIBUTE_DURATION;

	return 0;
}