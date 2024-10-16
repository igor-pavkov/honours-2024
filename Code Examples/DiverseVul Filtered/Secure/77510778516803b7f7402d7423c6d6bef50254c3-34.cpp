GF_Err ainf_box_size(GF_Box *s)
{
	GF_AssetInformationBox *ptr = (GF_AssetInformationBox *) s;
    s->size += 4 + (ptr->APID ? strlen(ptr->APID) : 0 ) + 1;
	return GF_OK;
}