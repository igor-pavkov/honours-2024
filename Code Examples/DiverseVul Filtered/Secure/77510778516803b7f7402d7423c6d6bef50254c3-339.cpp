GF_Box *pcmC_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_PCMConfigBox, GF_ISOM_BOX_TYPE_PCMC);
	return (GF_Box *)tmp;
}