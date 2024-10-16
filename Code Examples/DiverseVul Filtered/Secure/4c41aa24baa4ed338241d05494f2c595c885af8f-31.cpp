int ncp_search_for_fileset(struct ncp_server *server,
			   struct nw_search_sequence *seq,
			   int* more,
			   int* cnt,
			   char* buffer,
			   size_t bufsize,
			   char** rbuf,
			   size_t* rsize)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 20);
	ncp_add_byte(server, server->name_space[seq->volNumber]);
	ncp_add_byte(server, 0);		/* datastream */
	ncp_add_word(server, cpu_to_le16(0x8006));
	ncp_add_dword(server, RIM_ALL);
	ncp_add_word(server, cpu_to_le16(32767));	/* max returned items */
	ncp_add_mem(server, seq, 9);
#ifdef CONFIG_NCPFS_NFS_NS
	if (server->name_space[seq->volNumber] == NW_NS_NFS) {
		ncp_add_byte(server, 0);	/* 0 byte pattern */
	} else 
#endif
	{
		ncp_add_byte(server, 2);	/* 2 byte pattern */
		ncp_add_byte(server, 0xff);	/* following is a wildcard */
		ncp_add_byte(server, '*');
	}
	result = ncp_request2(server, 87, buffer, bufsize);
	if (result) {
		ncp_unlock_server(server);
		return result;
	}
	if (server->ncp_reply_size < 12) {
		ncp_unlock_server(server);
		return 0xFF;
	}
	*rsize = server->ncp_reply_size - 12;
	ncp_unlock_server(server);
	buffer = buffer + sizeof(struct ncp_reply_header);
	*rbuf = buffer + 12;
	*cnt = WVAL_LH(buffer + 10);
	*more = BVAL(buffer + 9);
	memcpy(seq, buffer, 9);
	return 0;
}