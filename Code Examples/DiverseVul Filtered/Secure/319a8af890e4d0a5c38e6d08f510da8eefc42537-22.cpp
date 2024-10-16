static void parea_report (gerbv_net_t *net,
		gerbv_image_t *img, gerbv_project_t *prj)
{
	gerbv_net_t *n;
	unsigned int c = 0;
	gerbv_interpolation_t inter_prev;
	double x, y;

	if (net->interpolation != GERBV_INTERPOLATION_PAREA_START)
		return;

	/* Count vertices */
	for (gerbv_net_t *n = net->next; n != NULL; n = n->next) {
		if (n->interpolation == GERBV_INTERPOLATION_PAREA_END)
			break;
		c++;
	}

	g_message (_("    Number of vertices: %u"), c - 1);

	for (n = net->next, inter_prev = net->interpolation;
			n != NULL
			&& n->interpolation != GERBV_INTERPOLATION_PAREA_END;
			n = n->next) {

		switch (n->interpolation) {

		case GERBV_INTERPOLATION_LINEARx1:

			if (inter_prev != n->interpolation) {
				x = n->start_x;
				y = n->start_y;
				gerbv_transform_coord_for_image(&x, &y,
						img, prj);
				g_message (_("    Line from: (%g, %g) %s"),
						screen_units(x),
						screen_units(y),
						screen_units_str());
			}

			x = n->stop_x;
			y = n->stop_y;
			gerbv_transform_coord_for_image(&x, &y, img, prj);
			g_message (_("        Line to: (%g, %g) %s"),
					screen_units(x), screen_units(y),
					screen_units_str());
			break;

		case GERBV_INTERPOLATION_CW_CIRCULAR:
		case GERBV_INTERPOLATION_CCW_CIRCULAR:

			x = n->start_x;
			y = n->start_y;
			gerbv_transform_coord_for_image(&x, &y, img, prj);
			g_message (_("    Arc from: (%g, %g) %s"),
					screen_units(x), screen_units(y),
					screen_units_str());

			x = n->stop_x;
			y = n->stop_y;
			gerbv_transform_coord_for_image(&x, &y, img, prj);
			g_message (_("        Arc to: (%g, %g) %s"),
					screen_units(x), screen_units(y),
					screen_units_str());

			x = n->cirseg->cp_x;
			y = n->cirseg->cp_y;
			gerbv_transform_coord_for_image(&x, &y, img, prj);
			g_message (_("        Center: (%g, %g) %s"),
					screen_units(x), screen_units(y),
					screen_units_str());

			x = n->cirseg->width;
			y = n->cirseg->height;
			gerbv_transform_coord_for_image(&x, &y, img, prj);
			g_message (_("        Radius: %g %s"),
					screen_units(x)/2, screen_units_str());

			g_message (_("        Angle: %g deg"),
				fabs(n->cirseg->angle1 - n->cirseg->angle2));
			g_message (_("        Angles: (%g, %g) deg"),
					n->cirseg->angle1, n->cirseg->angle2);
			g_message (_("        Direction: %s"),
					(n->interpolation ==
					 GERBV_INTERPOLATION_CW_CIRCULAR)?
						_("CW"): _("CCW"));
			break;

		default:
			g_message("       Skipping interpolation: %s",
				_(gerbv_interpolation_name(n->interpolation)));
		}

		inter_prev = n->interpolation;
	}
}