GF_Box *tcmi_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TimeCodeMediaInformationBox, GF_QT_BOX_TYPE_TMCD);
	tmp->text_size = 12;
	tmp->text_color_red = 0xFFFF;
	tmp->text_color_green = 0xFFFF;
	tmp->text_color_blue = 0xFFFF;
	return (GF_Box *)tmp;
}