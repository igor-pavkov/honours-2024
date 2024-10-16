void trak_del(GF_Box *s)
{
	GF_TrackBox *ptr = (GF_TrackBox *) s;
	if (ptr == NULL) return;

	if (ptr->Header) gf_isom_box_del((GF_Box *)ptr->Header);
	if (ptr->udta) gf_isom_box_del((GF_Box *)ptr->udta);
	if (ptr->Media) gf_isom_box_del((GF_Box *)ptr->Media);
	if (ptr->References) gf_isom_box_del((GF_Box *)ptr->References);
	if (ptr->editBox) gf_isom_box_del((GF_Box *)ptr->editBox);
	if (ptr->meta) gf_isom_box_del((GF_Box *)ptr->meta);
	if (ptr->name) gf_free(ptr->name);
	if (ptr->groups) gf_isom_box_del((GF_Box *)ptr->groups);
	if (ptr->Aperture) gf_isom_box_del((GF_Box *)ptr->Aperture);
	gf_free(ptr);
}