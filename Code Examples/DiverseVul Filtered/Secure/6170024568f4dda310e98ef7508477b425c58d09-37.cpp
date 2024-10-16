void chan_box_del(GF_Box *s)
{
	GF_ChannelLayoutInfoBox *ptr = (GF_ChannelLayoutInfoBox *)s;
	if (ptr->audio_descs) gf_free(ptr->audio_descs);
	gf_free(s);
}