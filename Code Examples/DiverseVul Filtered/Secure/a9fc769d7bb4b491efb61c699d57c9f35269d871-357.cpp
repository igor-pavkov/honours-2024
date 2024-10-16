void proto_add_heuristic_dissector(protocol_t *protocol, const char *short_name)
{
	heur_dtbl_entry_t* heuristic_dissector;

	if (protocol == NULL)
		return;

	heuristic_dissector = find_heur_dissector_by_unique_short_name(short_name);
	if (heuristic_dissector != NULL)
	{
		protocol->heur_list = g_list_prepend (protocol->heur_list, heuristic_dissector);
	}
}