GF_Err schi_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_SchemeInformationBox *ptr = (GF_SchemeInformationBox *)s;

	gf_isom_check_position(s, (GF_Box *)ptr->ikms, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->isfm, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->islt, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->odkm, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->tenc, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->adkm, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->piff_tenc, &pos);
	return GF_OK;
}