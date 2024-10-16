int ftrace_text_reserved(void *start, void *end)
{
	unsigned long ret;

	ret = ftrace_location_range((unsigned long)start,
				    (unsigned long)end);

	return (int)!!ret;
}