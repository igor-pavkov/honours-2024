static int mount_cifs_usage(FILE * stream)
{
	fprintf(stream, "\nUsage:  %s <remotetarget> <dir> -o <options>\n",
		thisprogram);
	fprintf(stream, "\nMount the remote target, specified as a UNC name,");
	fprintf(stream, " to a local directory.\n\nOptions:\n");
	fprintf(stream, "\tuser=<arg>\n\tpass=<arg>\n\tdom=<arg>\n");
	fprintf(stream, "\nLess commonly used options:");
	fprintf(stream,
		"\n\tcredentials=<filename>,guest,perm,noperm,setuids,nosetuids,rw,ro,");
	fprintf(stream,
		"\n\tsep=<char>,iocharset=<codepage>,suid,nosuid,exec,noexec,serverino,");
	fprintf(stream,
		"\n\tmapchars,nomapchars,nolock,servernetbiosname=<SRV_RFC1001NAME>");
	fprintf(stream,
		"\n\tdirectio,nounix,cifsacl,sec=<authentication mechanism>,sign,fsc");
	fprintf(stream,
		"\n\nOptions not needed for servers supporting CIFS Unix extensions");
	fprintf(stream,
		"\n\t(e.g. unneeded for mounts to most Samba versions):");
	fprintf(stream,
		"\n\tuid=<uid>,gid=<gid>,dir_mode=<mode>,file_mode=<mode>,sfu");
	fprintf(stream, "\n\nRarely used options:");
	fprintf(stream,
		"\n\tport=<tcpport>,rsize=<size>,wsize=<size>,unc=<unc_name>,ip=<ip_address>,");
	fprintf(stream,
		"\n\tdev,nodev,nouser_xattr,netbiosname=<OUR_RFC1001NAME>,hard,soft,intr,");
	fprintf(stream,
		"\n\tnointr,ignorecase,noposixpaths,noacl,prefixpath=<path>,nobrl");
	fprintf(stream,
		"\n\nOptions are described in more detail in the manual page");
	fprintf(stream, "\n\tman 8 mount.cifs\n");
	fprintf(stream, "\nTo display the version number of the mount helper:");
	fprintf(stream, "\n\t%s -V\n", thisprogram);

	if (stream == stderr)
		return EX_USAGE;
	return 0;
}