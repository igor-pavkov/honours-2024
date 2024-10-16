void clear_globals(void)
{
	extern int ntp_optind;
	showhostnames = 0;	/* don'tshow host names by default */
	ntp_optind = 0;
	server_entry = NULL;	/* server entry for ntp */
	havehost = 0;		/* set to 1 when host open */
	numassoc = 0;		/* number of cached associations */
	numcmds = 0;
	numhosts = 0;
}