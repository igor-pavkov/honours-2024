GF_Box *tpay_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TPAYBox, GF_ISOM_BOX_TYPE_TPAY);
	return (GF_Box *)tmp;
}