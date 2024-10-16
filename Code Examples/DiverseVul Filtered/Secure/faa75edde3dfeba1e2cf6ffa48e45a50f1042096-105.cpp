void gf_laser_decoder_del(GF_LASeRCodec *codec)
{
	/*destroy all config*/
	while (gf_list_count(codec->streamInfo)) {
		LASeRStreamInfo *p = (LASeRStreamInfo *)gf_list_last(codec->streamInfo);
		gf_free(p);
		gf_list_rem_last(codec->streamInfo);
	}
	gf_list_del(codec->streamInfo);
	if (codec->col_table) gf_free(codec->col_table);
	while (gf_list_count(codec->font_table)) {
		char *ft = (char *)gf_list_last(codec->font_table);
		gf_free(ft);
		gf_list_rem_last(codec->font_table);
	}
	gf_list_del(codec->font_table);
#if 0
	while (gf_list_count(codec->deferred_hrefs)) {
		XMLRI *iri = (XMLRI *)gf_list_last(codec->deferred_hrefs);
		gf_list_rem_last(codec->deferred_hrefs);
		if (iri->string) gf_free(iri->string);
		iri->string = NULL;
	}
#endif
	gf_list_del(codec->deferred_hrefs);
	gf_list_del(codec->deferred_anims);
	gf_list_del(codec->deferred_listeners);
	gf_list_del(codec->unresolved_commands);
	gf_free(codec);
}