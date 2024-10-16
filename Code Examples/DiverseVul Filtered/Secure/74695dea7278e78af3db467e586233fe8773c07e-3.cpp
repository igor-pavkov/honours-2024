static void nhml_node_start(void *sax_cbck, const char *node_name, const char *name_space, const GF_XMLAttribute *attributes, u32 nb_attributes)
{
	XMLBreaker *breaker = (XMLBreaker *)sax_cbck;
	char *node_id;
	u32 i;
	node_id = NULL;
	for (i=0; i<nb_attributes; i++) {
		GF_XMLAttribute *att = (GF_XMLAttribute *) &attributes[i];
		if (stricmp(att->name, "DEF") && stricmp(att->name, "id")) continue;
		node_id = gf_strdup(att->value);
		break;
	}
	if (!node_id) {
		node_id = gf_strdup("__nhml__none");
		gf_list_add(breaker->id_stack, node_id);
		return;
	}
	gf_list_add(breaker->id_stack, node_id);

	if (breaker->from_is_start && breaker->from_id && !strcmp(breaker->from_id, node_id)) {
		breaker->from_pos = gf_xml_sax_get_node_start_pos(breaker->sax);
		breaker->from_is_start = GF_FALSE;
	}
	if (breaker->to_is_start && breaker->to_id && !strcmp(breaker->to_id, node_id)) {
		breaker->to_pos = gf_xml_sax_get_node_start_pos(breaker->sax);
		breaker->to_is_start = GF_FALSE;
	}
	if (!breaker->to_is_start && !breaker->from_is_start && !breaker->to_is_end && !breaker->from_is_end) {
		gf_xml_sax_suspend(breaker->sax, GF_TRUE);
	}

}