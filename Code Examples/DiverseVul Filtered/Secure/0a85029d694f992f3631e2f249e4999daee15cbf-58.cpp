GF_Box *m4ds_box_new()
{
	GF_MPEG4ExtensionDescriptorsBox *tmp = (GF_MPEG4ExtensionDescriptorsBox *) gf_malloc(sizeof(GF_MPEG4ExtensionDescriptorsBox));
	if (tmp == NULL) return NULL;
	memset(tmp, 0, sizeof(GF_MPEG4ExtensionDescriptorsBox));
	tmp->type = GF_ISOM_BOX_TYPE_M4DS;
	tmp->descriptors = gf_list_new();
	return (GF_Box *)tmp;
}