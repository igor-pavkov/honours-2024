static void drill_report(gerbv_aperture_t *apertures[], int aperture_num)
{
	gerbv_aperture_type_t type = apertures[aperture_num]->type;
	double *params = apertures[aperture_num]->parameter;

	g_message (_("    Tool used: T%d"), aperture_num);
	if (type == GERBV_APTYPE_CIRCLE)
		g_message (_("    Diameter: %g %s"),
				screen_units(params[0]),
				screen_units_str());
}