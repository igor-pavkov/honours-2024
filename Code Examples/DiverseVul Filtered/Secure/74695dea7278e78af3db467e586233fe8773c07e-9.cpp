static void nhml_node_end(void *sax_cbck, const char *node_name, const char *name_space)
{
	XMLBreaker *breaker = (XMLBreaker *)sax_cbck;
	char *node_id = (char *)gf_list_last(breaker->id_stack);
	gf_list_rem_last(breaker->id_stack);
	if (breaker->from_is_end && breaker->from_id && !strcmp(breaker->from_id, node_id)) {
		breaker->from_pos = gf_xml_sax_get_node_end_pos(breaker->sax);
		breaker->from_is_end = GF_FALSE;
	}
	if (breaker->to_is_end && breaker->to_id && !strcmp(breaker->to_id, node_id)) {
		breaker->to_pos = gf_xml_sax_get_node_end_pos(breaker->sax);
		breaker->to_is_end = GF_FALSE;
	}
	gf_free(node_id);
	if (!breaker->to_is_start && !breaker->from_is_start && !breaker->to_is_end && !breaker->from_is_end) {
		gf_xml_sax_suspend(breaker->sax, GF_TRUE);
	}
}