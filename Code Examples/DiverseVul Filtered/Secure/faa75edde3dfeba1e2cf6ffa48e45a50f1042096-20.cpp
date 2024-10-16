static void lsr_read_paint(GF_LASeRCodec *lsr, SVG_Paint *paint, const char *name)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 1, "hasIndex");
	if (val) {
		GF_LSR_READ_INT(lsr, val, lsr->colorIndexBits, name);
		lsr_get_color(lsr, val, &paint->color);
		paint->type = SVG_PAINT_COLOR;
		paint->color.type = 0;
	} else {
		GF_LSR_READ_INT(lsr, val, 2, "enum");
		switch (val) {
		case 0:
			GF_LSR_READ_INT(lsr, val, 2, "choice");
			switch (val) {
			case 0:
				paint->type = SVG_PAINT_INHERIT;
				break;
			case 1:
				paint->type = SVG_PAINT_COLOR;
				paint->color.type = SVG_COLOR_CURRENTCOLOR;
				break;
			default:
				paint->type = SVG_PAINT_NONE;
				break;
			}
			break;
		case 1:
		{
			XMLRI iri;
			memset(&iri, 0, sizeof(XMLRI));
			iri.type = 0xFF;
			lsr_read_any_uri(lsr, &iri, name);
			gf_node_unregister_iri(lsr->sg, &iri);
			gf_list_del_item(lsr->deferred_hrefs, &iri);

			paint->type = SVG_PAINT_URI;
			if (iri.string) {
				paint->type = SVG_PAINT_URI;
				paint->iri.type = XMLRI_STRING;
				paint->iri.string = iri.string;
			} else if (iri.target) {
				paint->iri.type = XMLRI_ELEMENTID;
				paint->iri.target = iri.target;
			}
		}
		break;
		case 2:
		{
			char *sysPaint=NULL;
			lsr_read_byte_align_string(lsr, &sysPaint, "systemsPaint");
			if (sysPaint) {
				paint->type = SVG_PAINT_COLOR;
				paint->color.type = gf_svg_get_system_paint_server_type(sysPaint);
				gf_free(sysPaint);
			}
		}
		break;
		case 3:
			lsr_read_extension(lsr, name);
			break;
		}
	}
}