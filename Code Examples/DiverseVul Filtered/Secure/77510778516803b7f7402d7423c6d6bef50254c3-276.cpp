GF_Box *pdin_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ProgressiveDownloadBox, GF_ISOM_BOX_TYPE_PDIN);
	return (GF_Box *)tmp;
}