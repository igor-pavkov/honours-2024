void proto_heuristic_dissector_foreach(const protocol_t *protocol, GFunc func, gpointer user_data)
{
	if (protocol == NULL)
		return;

	g_list_foreach(protocol->heur_list, func, user_data);
}