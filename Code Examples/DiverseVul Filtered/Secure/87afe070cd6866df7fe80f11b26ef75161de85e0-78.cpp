void scene_coding_log(void *cbk, GF_LOG_Level log_level, GF_LOG_Tool log_tool, const char *fmt, va_list vlist)
{
	FILE *logs = cbk;
	if (log_tool != GF_LOG_CODING) return;
	vfprintf(logs, fmt, vlist);
	fflush(logs);
}