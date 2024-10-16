GF_Err hinf_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MAXRBox *maxR;
	GF_HintInfoBox *hinf = (GF_HintInfoBox *)s;
	u32 i;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_MAXR:
		i=0;
		while ((maxR = (GF_MAXRBox *)gf_list_enum(hinf->child_boxes, &i))) {
			if ((maxR->type==GF_ISOM_BOX_TYPE_MAXR) && (maxR->granularity == ((GF_MAXRBox *)a)->granularity))
				ERROR_ON_DUPLICATED_BOX(a, s)
		}
		break;
	}
	return GF_OK;
}