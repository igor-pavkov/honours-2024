static GF_Err do_remux_file()
{
	GF_MediaExporter mdump;
	memset(&mdump, 0, sizeof(GF_MediaExporter));
	mdump.in_name = inName;
	mdump.out_name = mux_name;
	mdump.flags = GF_EXPORT_REMUX;
	mdump.print_stats_graph = fs_dump_flags;
	return gf_media_export(&mdump);
}