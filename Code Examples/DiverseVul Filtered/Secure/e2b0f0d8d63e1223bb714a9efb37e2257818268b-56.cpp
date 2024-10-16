static const char *subtype_to_string(uint32_t subtype)
{
	switch (subtype & 0x03) {
	case 0x01:
		return "Audio Book";
	case 0x02:
		return "Podcast";
	case 0x03:
		return "Audio Book, Podcast";
	}

	return "None";
}