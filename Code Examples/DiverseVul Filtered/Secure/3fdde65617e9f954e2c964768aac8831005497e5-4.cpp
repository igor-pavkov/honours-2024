static int exif_rewrite_tag_format_to_unsigned(int format)
{
	switch(format) {
		case TAG_FMT_SBYTE: return TAG_FMT_BYTE;
		case TAG_FMT_SRATIONAL: return TAG_FMT_URATIONAL;
		case TAG_FMT_SSHORT: return TAG_FMT_USHORT;
		case TAG_FMT_SLONG: return TAG_FMT_ULONG;
	}
	return format;
}