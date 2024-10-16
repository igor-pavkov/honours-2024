GF_Box *vmhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_VideoMediaHeaderBox, GF_ISOM_BOX_TYPE_VMHD);
	tmp->flags = 1;
	return (GF_Box *)tmp;
}