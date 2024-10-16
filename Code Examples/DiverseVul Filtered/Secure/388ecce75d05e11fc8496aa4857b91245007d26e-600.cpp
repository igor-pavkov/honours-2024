GF_Box *mdhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MediaHeaderBox, GF_ISOM_BOX_TYPE_MDHD);

	tmp->packedLanguage[0] = 'u';
	tmp->packedLanguage[1] = 'n';
	tmp->packedLanguage[2] = 'd';
	return (GF_Box *)tmp;
}