const char* rdpgfx_get_cmd_id_string(UINT16 cmdId)
{
	if (cmdId <= RDPGFX_CMDID_MAPSURFACETOSCALEDWINDOW)
		return RDPGFX_CMDID_STRINGS[cmdId];
	else
		return "RDPGFX_CMDID_UNKNOWN";
}