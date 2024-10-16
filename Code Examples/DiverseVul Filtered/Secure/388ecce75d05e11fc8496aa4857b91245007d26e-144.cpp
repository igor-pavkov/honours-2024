GF_Err minf_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;
	//Header first
	gf_isom_check_position(s, (GF_Box *)ptr->InfoHeader, &pos);
	//then dataInfo
	gf_isom_check_position(s, (GF_Box *)ptr->dataInformation, &pos);
	gf_isom_check_position(s, gf_isom_box_find_child(s->child_boxes, GF_ISOM_BOX_TYPE_MVCI), &pos);
	//then sampleTable
	gf_isom_check_position(s, (GF_Box *)ptr->sampleTable, &pos);
	return GF_OK;
}