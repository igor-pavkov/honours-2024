void moof_box_del(GF_Box *s)
{
	GF_MovieFragmentBox *ptr = (GF_MovieFragmentBox *)s;
	if (ptr == NULL) return;

	gf_list_del(ptr->TrackList);
	if (ptr->PSSHs) gf_list_del(ptr->PSSHs);
	if (ptr->mdat) gf_free(ptr->mdat);
	gf_free(ptr);
}