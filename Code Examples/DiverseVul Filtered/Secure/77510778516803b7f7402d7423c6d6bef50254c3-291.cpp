GF_Err sbgp_box_size(GF_Box *s)
{
	GF_SampleGroupBox *p = (GF_SampleGroupBox*)s;

	p->size += 8;
	if (p->grouping_type_parameter) p->version=1;

	if (p->version==1) p->size += 4;
	p->size += 8*p->entry_count;
	return GF_OK;
}