GF_Err audio_sample_entry_Size(GF_Box *s)
{
	GF_Err e;
	GF_MPEGAudioSampleEntryBox *ptr = (GF_MPEGAudioSampleEntryBox *)s;

	gf_isom_audio_sample_entry_size((GF_AudioSampleEntryBox*)s);

	if (ptr->is_qtff)
		return gf_isom_box_array_size(s, ptr->protections);

	if (ptr->esd) {
		e = gf_isom_box_size((GF_Box *)ptr->esd);
		if (e) return e;
		ptr->size += ptr->esd->size;
	}
	if (ptr->cfg_3gpp) {
		e = gf_isom_box_size((GF_Box *)ptr->cfg_3gpp);
		if (e) return e;
		ptr->size += ptr->cfg_3gpp->size;
	}
	if (ptr->cfg_opus) {
		e = gf_isom_box_size((GF_Box *)ptr->cfg_opus);
		if (e) return e;
		ptr->size += ptr->cfg_opus->size;
	}
	if (ptr->cfg_ac3) {
		e = gf_isom_box_size((GF_Box *)ptr->cfg_ac3);
		if (e) return e;
		ptr->size += ptr->cfg_ac3->size;
	}
	return gf_isom_box_array_size(s, ptr->protections);
}