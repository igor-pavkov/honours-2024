static int show_stat_exec(const char *cmd, char * const *fields, int fields_cnt, void *client)
{
	cli_send(client, "pptp:\r\n");
	cli_sendv(client,"  starting: %u\r\n", stat_starting);
	cli_sendv(client,"  active: %u\r\n", stat_active);

	return CLI_CMD_OK;
}