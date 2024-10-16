GF_Err gf_media_export(GF_MediaExporter *dumper)
{
	if (!dumper) return GF_BAD_PARAM;
	if (!dumper->out_name && !(dumper->flags & GF_EXPORT_PROBE_ONLY) && !dumper->dump_file) return GF_BAD_PARAM;

	//internal export not using filters

#ifndef GPAC_DISABLE_ISOM_WRITE
	if (dumper->flags & GF_EXPORT_MP4) return gf_media_export_isom(dumper);
#endif /*GPAC_DISABLE_ISOM_WRITE*/
#ifndef GPAC_DISABLE_VTT
	else if (dumper->flags & GF_EXPORT_WEBVTT_META) return gf_media_export_webvtt_metadata(dumper);
#endif
	else if (dumper->flags & GF_EXPORT_SIX) return gf_media_export_six(dumper);

	//the following ones should be moved to muxing filters
	else if (dumper->flags & GF_EXPORT_SAF) return gf_media_export_saf(dumper);

	//the rest is handled by the generic exporter
	return gf_media_export_filters(dumper);
}