static void cmd_mode(char *arg)
{
    lcase(arg);

    if (!strcmp(arg, "reader")) {
	prot_printf(nntp_out, "%u", (nntp_capa & MODE_READ) ? 200 : 201);
	if (config_serverinfo || nntp_authstate) {
	    prot_printf(nntp_out, " %s", config_servername);
	}
	if (nntp_authstate || (config_serverinfo == IMAP_ENUM_SERVERINFO_ON)) {
	    prot_printf(nntp_out, " Cyrus NNTP%s %s",
			config_mupdate_server ? " Murder" : "", cyrus_version());
	}
	prot_printf(nntp_out, " server ready, posting %s\r\n",
		    (nntp_capa & MODE_READ) ? "allowed" : "prohibited");
    }
    else if (!strcmp(arg, "stream")) {
	if (nntp_capa & MODE_FEED) {
	    prot_printf(nntp_out, "203 Streaming allowed\r\n");
	}
	else {
	    prot_printf(nntp_out, "502 Streaming prohibited\r\n");
	}
    }
    else {
	prot_printf(nntp_out, "501 Unrecognized MODE\r\n");
    }
    prot_flush(nntp_out);
}