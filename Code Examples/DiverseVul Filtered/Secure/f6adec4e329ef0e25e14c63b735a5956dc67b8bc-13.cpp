static void warn_command_line_option(const char *var, const char *value)
{
	warning(_("ignoring '%s' which may be interpreted as"
		  " a command-line option: %s"), var, value);
}