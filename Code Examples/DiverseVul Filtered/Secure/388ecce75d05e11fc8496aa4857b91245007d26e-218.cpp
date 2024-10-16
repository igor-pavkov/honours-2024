GF_Err mp4s_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MPEGSampleEntryBox *ptr = (GF_MPEGSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_ESDS:
		if (ptr->esd) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->esd = (GF_ESDBox *)a;
		break;
	}
	return GF_OK;
}