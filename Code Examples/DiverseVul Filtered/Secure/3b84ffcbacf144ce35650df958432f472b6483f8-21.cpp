GF_ISMASample *gf_isom_ismacryp_new_sample()
{
	GF_ISMASample *tmp = (GF_ISMASample *) gf_malloc(sizeof(GF_ISMASample));
	if (!tmp) return NULL;
	memset(tmp, 0, sizeof(GF_ISMASample));
	return tmp;
}