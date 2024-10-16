GF_Err CoLL_box_size(GF_Box *s)
{
	GF_VPContentLightLevelBox *p = (GF_VPContentLightLevelBox*)s;
	p->size += 4;
	return GF_OK;
}