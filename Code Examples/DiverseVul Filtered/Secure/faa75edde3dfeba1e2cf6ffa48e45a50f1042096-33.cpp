GF_LASeRCodec *gf_laser_decoder_new(GF_SceneGraph *graph)
{
	GF_LASeRCodec *tmp;
	GF_SAFEALLOC(tmp, GF_LASeRCodec);
	if (!tmp) return NULL;
	tmp->streamInfo = gf_list_new();
	tmp->font_table = gf_list_new();
	tmp->deferred_hrefs = gf_list_new();
	tmp->deferred_listeners = gf_list_new();
	tmp->deferred_anims = gf_list_new();
	tmp->unresolved_commands = gf_list_new();
	tmp->sg = graph;
	return tmp;
}