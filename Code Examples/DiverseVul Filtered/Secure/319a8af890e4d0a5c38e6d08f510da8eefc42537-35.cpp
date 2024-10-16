static void aperture_state_report (gerbv_net_t *net,
		gerbv_image_t *img, gerbv_project_t *prj)
{
	gerbv_layertype_t layer_type = img->layertype;

	gboolean show_length = FALSE;
	gboolean aperture_is_valid = FALSE;
	double x, y, len = 0;

	if (net->aperture > 0)
		aperture_is_valid = TRUE;

	switch (net->aperture_state) {

	case GERBV_APERTURE_STATE_OFF:
		break;

	case GERBV_APERTURE_STATE_ON:
		switch (net->interpolation) {

		case GERBV_INTERPOLATION_LINEARx1:
		case GERBV_INTERPOLATION_LINEARx10:
		case GERBV_INTERPOLATION_LINEARx01:
		case GERBV_INTERPOLATION_LINEARx001:
			if (layer_type != GERBV_LAYERTYPE_DRILL)
				g_message (_("Object type: Line"));
			else
				g_message (_("Object type: Slot (drilled)"));

			len = line_length(net->start_x, net->start_y,
					net->stop_x, net->stop_y);
			show_length = 1;

			break;

		case GERBV_INTERPOLATION_CW_CIRCULAR:
		case GERBV_INTERPOLATION_CCW_CIRCULAR:
			g_message (_("Object type: Arc"));
			len = arc_length(net->cirseg->width,
					fabs(net->cirseg->angle1 -
						net->cirseg->angle2));
			show_length = 1;

			break;
		default:
			g_message (_("Object type: Unknown"));
			break;
		}

		if (layer_type != GERBV_LAYERTYPE_DRILL)
			g_message (_("    Exposure: On"));

		if (aperture_is_valid) {
			if (layer_type != GERBV_LAYERTYPE_DRILL)
				aperture_report(img->aperture, net->aperture,
						net->start_x, net->start_y,
						img, prj);
			else
				drill_report(img->aperture, net->aperture);
		}

		x = net->start_x;
		y = net->start_y;
		gerbv_transform_coord_for_image(&x, &y, img, prj);
		g_message (_("    Start: (%g, %g) %s"),
				screen_units(x),
				screen_units(y),
				screen_units_str());

		x = net->stop_x;
		y = net->stop_y;
		gerbv_transform_coord_for_image(&x, &y, img, prj);
		g_message (_("    Stop: (%g, %g) %s"),
				screen_units(x),
				screen_units(y),
				screen_units_str());

		switch (net->interpolation) {

		case GERBV_INTERPOLATION_CW_CIRCULAR:
		case GERBV_INTERPOLATION_CCW_CIRCULAR:
			x = net->cirseg->cp_x;
			y = net->cirseg->cp_y;
			gerbv_transform_coord_for_image(&x, &y, img, prj);
			g_message (_("    Center: (%g, %g) %s"),
					screen_units(x),
					screen_units(y),
					screen_units_str());

			x = net->cirseg->width/2;
			y = x;
			gerbv_transform_coord_for_image(&x, &y, img, prj);
			g_message (_("    Radius: %g %s"),
					screen_units(x),
					screen_units_str());

			g_message (_("    Angle: %g deg"),
					fabs(net->cirseg->angle1 -
						net->cirseg->angle2));
			g_message (_("    Angles: (%g, %g) deg"),
					net->cirseg->angle1,
					net->cirseg->angle2);
			g_message (_("    Direction: %s"),
					(net->interpolation ==
					 GERBV_INTERPOLATION_CW_CIRCULAR)?
						_("CW"): _("CCW"));
			break;

		default:
			break;
		}

		if (show_length) {
			gerbv_aperture_t *aper = img->aperture[net->aperture];

			if (layer_type == GERBV_LAYERTYPE_DRILL
			&&  aperture_is_valid
			&&  aper->type == GERBV_APTYPE_CIRCLE) {
				double dia = aper->parameter[0];
				g_message (_("    Slot length: %g %s"),
						screen_units(len + dia),
						screen_units_str());
			}

			screen.length_sum += len;
			g_message (_("    Length: %g (sum: %g) %s"),
					screen_units(len),
					screen_units(screen.length_sum),
					screen_units_str());
		}

		net_layer_file_report (net, img, prj);

		break;

	case GERBV_APERTURE_STATE_FLASH:
		if (layer_type != GERBV_LAYERTYPE_DRILL)
			g_message (_("Object type: Flashed aperture"));
		else
			g_message (_("Object type: Drill"));

		if (aperture_is_valid) {
			if (layer_type != GERBV_LAYERTYPE_DRILL)
				aperture_report(img->aperture, net->aperture,
						net->stop_x, net->stop_y,
						img, prj);
			else
				drill_report(img->aperture, net->aperture);
		}

		x = net->stop_x;
		y = net->stop_y;
		gerbv_transform_coord_for_image(&x, &y, img, prj);
		g_message (_("    Location: (%g, %g) %s"),
				screen_units(x),
				screen_units(y),
				screen_units_str());

		net_layer_file_report (net, img, prj);

		break;
	}
}