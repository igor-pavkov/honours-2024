GF_Box *stbl_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleTableBox, GF_ISOM_BOX_TYPE_STBL);
	//maxSamplePer chunk is 10 by default
	tmp->MaxSamplePerChunk = 10;
	tmp->groupID = 1;
	return (GF_Box *)tmp;
}