GF_Box *cprt_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_CopyrightBox, GF_ISOM_BOX_TYPE_CPRT);
	tmp->packedLanguageCode[0] = 'u';
	tmp->packedLanguageCode[1] = 'n';
	tmp->packedLanguageCode[2] = 'd';

	return (GF_Box *)tmp;
}