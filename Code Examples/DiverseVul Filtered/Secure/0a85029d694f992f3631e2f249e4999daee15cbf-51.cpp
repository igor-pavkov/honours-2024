void CoLL_box_del(GF_Box *a)
{
	GF_VPContentLightLevelBox *p = (GF_VPContentLightLevelBox *)a;
	gf_free(p);
}