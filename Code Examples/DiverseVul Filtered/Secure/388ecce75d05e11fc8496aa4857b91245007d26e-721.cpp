GF_Box *stdp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DegradationPriorityBox, GF_ISOM_BOX_TYPE_STDP);
	return (GF_Box *)tmp;
}