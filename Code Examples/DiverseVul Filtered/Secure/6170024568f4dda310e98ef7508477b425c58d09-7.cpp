GF_Err chan_box_size(GF_Box *s)
{
	GF_ChannelLayoutInfoBox *ptr = (GF_ChannelLayoutInfoBox *)s;
	s->size += 12 + 20 * ptr->num_audio_description;
	return GF_OK;
}