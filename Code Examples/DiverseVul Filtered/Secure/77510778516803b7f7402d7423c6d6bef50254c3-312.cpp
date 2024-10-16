GF_Err lsr1_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_LSRC:
		BOX_FIELD_ASSIGN(lsr_config, GF_LASERConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_M4DS:
		BOX_FIELD_ASSIGN(descr, GF_MPEG4ExtensionDescriptorsBox)
		break;
	}
	return GF_OK;
}