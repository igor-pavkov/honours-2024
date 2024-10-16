static GF_Err gf_export_message(GF_MediaExporter *dumper, GF_Err e, char *format, ...)
{
	if (dumper->flags & GF_EXPORT_PROBE_ONLY) return e;

#ifndef GPAC_DISABLE_LOG
	if (gf_log_tool_level_on(GF_LOG_AUTHOR, e ? GF_LOG_ERROR : GF_LOG_WARNING)) {
		va_list args;
		char szMsg[1024];
		va_start(args, format);
		vsnprintf(szMsg, 1024, format, args);
		va_end(args);
		GF_LOG((u32) (e ? GF_LOG_ERROR : GF_LOG_WARNING), GF_LOG_AUTHOR, ("%s\n", szMsg) );
	}
#endif
	return e;
}