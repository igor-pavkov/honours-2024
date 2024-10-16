Bool gf_isom_is_JPEG2000(GF_ISOFile *mov)
{
	return (mov && mov->is_jp2) ? GF_TRUE : GF_FALSE;
}