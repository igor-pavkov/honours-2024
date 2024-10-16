static GF_Err nhml_sample_from_xml(GF_NHMLDmxCtx *ctx, char *xml_file, char *xmlFrom, char *xmlTo)
{
	GF_Err e = GF_OK;
	u32 read;
	XMLBreaker breaker;
	char *tmp;
	FILE *xml;
	u8 szBOM[3];
	if (!xml_file || !xmlFrom || !xmlTo) return GF_BAD_PARAM;

	memset(&breaker, 0, sizeof(XMLBreaker));

	xml = gf_fopen(xml_file, "rb");
	if (!xml) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] import failure: file %s not found", xml_file ));
		goto exit;
	}
	//we cannot use files with BOM since the XML position we get from the parser are offsets in the UTF-8 version of the XML.
	//TODO: to support files with BOM we would need to serialize on the fly the callback from the sax parser
	read = (u32) gf_fread(szBOM, 3, xml);
	if (read==3) {
		gf_fseek(xml, 0, SEEK_SET);
		if ((szBOM[0]==0xFF) || (szBOM[0]==0xFE) || (szBOM[0]==0xEF)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] import failure: XML file %s uses unsupported BOM, please convert to plain UTF-8 or ANSI first", xml_file));
			goto exit;
		}
	}


	memset(&breaker, 0, sizeof(XMLBreaker));
	breaker.id_stack = gf_list_new();

	if (strstr(xmlFrom, ".start")) breaker.from_is_start = GF_TRUE;
	else breaker.from_is_end = GF_TRUE;
	tmp = strchr(xmlFrom, '.');
	*tmp = 0;
	if (stricmp(xmlFrom, "doc")) breaker.from_id = gf_strdup(xmlFrom);
	/*doc start pos is 0, no need to look for it*/
	else if (breaker.from_is_start) breaker.from_is_start = GF_FALSE;
	*tmp = '.';

	if (strstr(xmlTo, ".start")) breaker.to_is_start = GF_TRUE;
	else breaker.to_is_end = GF_TRUE;
	tmp = strchr(xmlTo, '.');
	*tmp = 0;
	if (stricmp(xmlTo, "doc")) breaker.to_id = gf_strdup(xmlTo);
	/*doc end pos is file size, no need to look for it*/
	else if (breaker.to_is_end) breaker.to_is_end = GF_FALSE;
	*tmp = '.';

	breaker.sax = gf_xml_sax_new(nhml_node_start, nhml_node_end, NULL, &breaker);
	e = gf_xml_sax_parse_file(breaker.sax, xml_file, NULL);
	gf_xml_sax_del(breaker.sax);
	if (e<0) goto exit;

	if (!breaker.to_id) {
		breaker.to_pos = gf_fsize(xml);
	}
	if (breaker.to_pos < breaker.from_pos) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] import failure: xmlFrom %s is located after xmlTo %s", xmlFrom, xmlTo));
		goto exit;
	}

	assert(breaker.to_pos > breaker.from_pos);


	ctx->samp_buffer_size = (u32) (breaker.to_pos - breaker.from_pos);
	if (ctx->samp_buffer_alloc < ctx->samp_buffer_size) {
		ctx->samp_buffer_alloc = ctx->samp_buffer_size;
		ctx->samp_buffer = (char*)gf_realloc(ctx->samp_buffer, sizeof(char)*ctx->samp_buffer_alloc);
	}
	gf_fseek(xml, breaker.from_pos, SEEK_SET);
	if (0 == gf_fread(ctx->samp_buffer, ctx->samp_buffer_size, xml)) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[NHMLDmx] Failed to read samp->dataLength\n"));
	}
	e = GF_OK;
	
exit:
	if (xml) gf_fclose(xml);
	while (gf_list_count(breaker.id_stack)) {
		char *id = (char *)gf_list_last(breaker.id_stack);
		gf_list_rem_last(breaker.id_stack);
		gf_free(id);
	}
	gf_list_del(breaker.id_stack);
	if (breaker.from_id) gf_free(breaker.from_id);
	if (breaker.to_id) gf_free(breaker.to_id);
	return e;
}