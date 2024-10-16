GF_Box *dec3_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AC3ConfigBox, GF_ISOM_BOX_TYPE_DAC3);
	tmp->cfg.is_ec3 = 1;
	return (GF_Box *)tmp;
}