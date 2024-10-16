static const char *type_to_string(uint8_t type)
{
	switch (type & 0x0F) {
	case 0x01:
		return "Audio";
	case 0x02:
		return "Video";
	case 0x03:
		return "Audio, Video";
	case 0x04:
		return "Audio Broadcasting";
	case 0x05:
		return "Audio, Audio Broadcasting";
	case 0x06:
		return "Video, Audio Broadcasting";
	case 0x07:
		return "Audio, Video, Audio Broadcasting";
	case 0x08:
		return "Video Broadcasting";
	case 0x09:
		return "Audio, Video Broadcasting";
	case 0x0A:
		return "Video, Video Broadcasting";
	case 0x0B:
		return "Audio, Video, Video Broadcasting";
	case 0x0C:
		return "Audio Broadcasting, Video Broadcasting";
	case 0x0D:
		return "Audio, Audio Broadcasting, Video Broadcasting";
	case 0x0E:
		return "Video, Audio Broadcasting, Video Broadcasting";
	case 0x0F:
		return "Audio, Video, Audio Broadcasting, Video Broadcasting";
	}

	return "None";
}