static const char *screen_units_str(void)
{
	/* NOTE: in order of gerbv_gui_unit_t */
	const char *units_str[] = {N_("mil"), N_("mm"), N_("in")};

	return _(units_str[screen.unit]);
}