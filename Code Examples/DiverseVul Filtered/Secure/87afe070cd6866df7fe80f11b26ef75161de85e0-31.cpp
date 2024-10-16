void PrintHintUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# Hinting Options\n"
		"IsoMedia hinting consists in creating special tracks in the file that contain transport protocol specific information and optionally multiplexing information. These tracks are then used by the server to create the actual packets being sent over the network, in other words they provide the server with hints on how to build packets, hence their names `hint tracks`.\n"
		"MP4Box supports creation of hint tracks for RTSP servers supporting these such as QuickTime Streaming Server, DarwinStreaming Server or 3GPP-compliant RTSP servers.\n"
		"Note: GPAC streaming tools [rtp output](rtpout) and [rtsp server](rtspout) do not use hint tracks, they use on-the-fly packetization "
		"from any media sources, not just MP4\n"
		"  \n"
		"Options:\n"
	);
	while (m4b_hint_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_hint_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-hint");
	}
}