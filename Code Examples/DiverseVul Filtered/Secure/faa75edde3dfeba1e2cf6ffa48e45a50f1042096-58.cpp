static void lsr_delete_anim_value(GF_LASeRCodec *lsr, SMIL_AnimateValue *val, u32 coded_type)
{
	//unable to transform, free mem and reset
	switch (coded_type) {
	case 0://SVG_string *
		gf_free(* (SVG_String *)val->value);
		gf_free(val->value);
		break;
	case 1://SVG_Number *
		gf_free(val->value);
		break;
	case 2: //SVG_PathData *
		gf_svg_delete_attribute_value(SVG_PathData_datatype, val->value, NULL);
		break;
	case 3: //SVG_Points *pts
		gf_svg_delete_attribute_value(SVG_Points_datatype, val->value, NULL);
		break;
	case 4: //SVG_Number*
		gf_free(val->value);
		break;
	case 5://SVG_Paint
		gf_free(val->value);
		break;
	case 6://u8*
		gf_free(val->value);
		break;
	case 7: //list of u8 *
	case 8: //list of floats
		{
			GF_List *l = (GF_List *) val->value;
			while (gf_list_count(l)) {
				void *v = gf_list_pop_back(l);
				gf_free(v);
			}
			gf_list_del(l);
		}
		break;
	/*point */
	case 9: //SVG_Point *
		gf_free(val->value);
		break;
	case 10: //u32 *
		gf_free(val->value);
		break;
	case 11: //SVG_FontFamily *
	{
		SVG_FontFamily *ft = (SVG_FontFamily *) val->value;
		if (ft->value) gf_free(ft->value);
		gf_free(ft);
	}
		break;
	case 12:
	{
		XMLRI *iri = (XMLRI *)val->value;
		gf_list_del_item(lsr->deferred_hrefs, iri);
		gf_node_unregister_iri(lsr->sg, iri);
		if (iri->string) gf_free(iri->string);
		gf_free(iri);
	}
	default:
		break;
	}
	val->value = NULL;
}