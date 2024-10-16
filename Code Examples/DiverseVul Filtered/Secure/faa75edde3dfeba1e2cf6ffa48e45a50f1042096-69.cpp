static void lsr_translate_anim_trans_value(GF_LASeRCodec *lsr, SMIL_AnimateValue *val, u32 transform_type)
{
	SVG_Point_Angle *p;
	Fixed *f;
	u32 coded_type = val->type;

	switch(transform_type) {
	case SVG_TRANSFORM_TRANSLATE:
		val->type = SVG_Transform_Translate_datatype;
		break;
	case SVG_TRANSFORM_SCALE:
		val->type = SVG_Transform_Scale_datatype;
		break;
	case SVG_TRANSFORM_ROTATE:
		val->type = SVG_Transform_Rotate_datatype;
		break;
	case SVG_TRANSFORM_SKEWX:
		val->type = SVG_Transform_SkewX_datatype;
		break;
	case SVG_TRANSFORM_SKEWY:
		val->type = SVG_Transform_SkewY_datatype;
		break;
	case SVG_TRANSFORM_MATRIX:
		val->type = SVG_Transform_datatype;
		break;
	default:
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[LSR Parsing] unknown datatype for animate transform.\n"));
		return;
	}
	if (!val->value) return;
	switch (transform_type) {
	case SVG_TRANSFORM_ROTATE:
		if (coded_type==8) {
			p = (SVG_Point_Angle*)gf_malloc(sizeof(SVG_Point_Angle));
			if (!p) {
				lsr->last_error = GF_OUT_OF_MEM;
				break;
			}
			p->x = p->y = 0;
			GF_List *l = (GF_List *)val->value;
			f = (Fixed*)gf_list_get(l, 0);
			if (f) p->angle = *f;

			f = (Fixed*)gf_list_get(l, 1);
			if (f) p->x = *f;

			f = (Fixed*)gf_list_get(l, 2);
			if (f) p->y = *f;

			while (gf_list_count(l)) {
				f = gf_list_pop_back(l);
				gf_free(f);
			}
			gf_list_del(l);
			p->angle = gf_muldiv(p->angle, GF_PI, INT2FIX(180) );
			val->value = p;
			return;
		} else if ((coded_type==1) || (coded_type==4)) {
			p = (SVG_Point_Angle*)gf_malloc(sizeof(SVG_Point_Angle));
			if (!p) {
				lsr->last_error = GF_OUT_OF_MEM;
				break;
			}
			p->x = p->y = 0;
			p->angle = ((SVG_Number *)val->value)->value;
			gf_free(val->value);
			p->angle = gf_muldiv(p->angle, GF_PI, INT2FIX(180) );
			val->value = p;
			return;
		}
		break;
	case SVG_TRANSFORM_SCALE:
		if (coded_type==8) {
			SVG_Point *pt;
			GF_List *l = (GF_List *)val->value;
			GF_SAFEALLOC(pt , SVG_Point);
			if (!pt) {
				lsr->last_error = GF_OUT_OF_MEM;
				break;
			}
			f = (Fixed*)gf_list_get(l, 0);
			if (f) pt->x = *f;
			f = (Fixed*)gf_list_get(l, 1);
			if (f) pt->y = *f;
			else pt->y = pt->x;

			while (gf_list_count(l)) {
				f = gf_list_pop_back(l);
				gf_free(f);
			}
			gf_list_del(l);
			val->value = pt;
			return;
		}
		break;
	case SVG_TRANSFORM_SKEWX:
	case SVG_TRANSFORM_SKEWY:
		if ((coded_type==1) || (coded_type==4)) {
			f = (Fixed*)gf_malloc(sizeof(Fixed));
			if (!f) {
				lsr->last_error = GF_OUT_OF_MEM;
				break;
			}
			*f = ((SVG_Number *)val->value)->value;
			gf_free(val->value);
			val->value = f;
			return;
		}
		break;
	}
	//not handled
	lsr_delete_anim_value(lsr, val, coded_type);
}