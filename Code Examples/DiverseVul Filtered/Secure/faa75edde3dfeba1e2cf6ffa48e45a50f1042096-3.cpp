static void lsr_translate_anim_trans_values(GF_LASeRCodec *lsr, SMIL_AnimateValues *val, u32 transform_type)
{
	u32 count, i, coded_type;
	SVG_Point_Angle *p;
	SVG_Point *pt;
	Fixed *f;
	GF_List *l;
	Bool handled = GF_FALSE;

	coded_type = val->type;
	switch(transform_type) {
	case SVG_TRANSFORM_TRANSLATE:
		val->type = SVG_Transform_Translate_datatype;
		break;
	case SVG_TRANSFORM_SCALE:
		if (coded_type==8) handled = GF_TRUE;
		val->type = SVG_Transform_Scale_datatype;
		break;
	case SVG_TRANSFORM_ROTATE:
		if ((coded_type==8) || (coded_type==1)) handled = GF_TRUE;
		val->type = SVG_Transform_Rotate_datatype;
		break;
	case SVG_TRANSFORM_SKEWX:
		if ((coded_type==1) || (coded_type==4)) handled = GF_TRUE;
		val->type = SVG_Transform_SkewX_datatype;
		break;
	case SVG_TRANSFORM_SKEWY:
		if ((coded_type==1) || (coded_type==4)) handled = GF_TRUE;
		val->type = SVG_Transform_SkewY_datatype;
		break;
	case SVG_TRANSFORM_MATRIX:
		val->type = SVG_Transform_datatype;
		break;
	default:
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[SVG Parsing] unknown datatype for animate transform.\n"));
		break;
	}

	if (!handled) {
		while (gf_list_count(val->values)) {
			SMIL_AnimateValue a_val;
			a_val.type = 0;
			a_val.value = gf_list_pop_back(val->values);
			lsr_delete_anim_value(lsr, &a_val, coded_type);
		}
		return;
	}

	count = gf_list_count(val->values);
	if (!count) return;

	if (transform_type==SVG_TRANSFORM_TRANSLATE)
		return;

	for (i=0; i<count; i++) {
		void *a_val = gf_list_get(val->values, i);
		switch (transform_type) {
		case SVG_TRANSFORM_ROTATE:
			GF_SAFEALLOC(p, SVG_Point_Angle);
			if (!p) {
				lsr->last_error = GF_OUT_OF_MEM;
				return;
			}
			if (coded_type==8) {
				l = (GF_List*)a_val;
				f = (Fixed*)gf_list_get(l, 0);
				if (f) p->angle = *f;
				f = (Fixed*)gf_list_get(l, 1);
				if (f) p->x = *f;
				f = (Fixed*)gf_list_get(l, 2);
				if (f) p->y = *f;
				while (gf_list_count(l)) {
					f = (Fixed*)gf_list_last(l);
					gf_list_rem_last(l);
					gf_free(f);
				}
				gf_list_del(l);
			} else if (coded_type==1) {
				p->angle = ((SVG_Number *)a_val)->value;
				gf_free(a_val);
			}
			p->angle = gf_muldiv(p->angle, GF_PI, INT2FIX(180) );
			gf_list_rem(val->values, i);
			gf_list_insert(val->values, p, i);
			break;
		case SVG_TRANSFORM_SKEWX:
		case SVG_TRANSFORM_SKEWY:
			f = (Fixed*)gf_malloc(sizeof(Fixed));
			if (!f) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				*f = ((SVG_Number *)a_val)->value;
			}
			gf_free(a_val);
			gf_list_rem(val->values, i);
			if (f)
				gf_list_insert(val->values, f, i);
			break;
		case SVG_TRANSFORM_SCALE:
			GF_SAFEALLOC(pt, SVG_Point);
			if (!pt)
				lsr->last_error = GF_OUT_OF_MEM;

			l = (GF_List*)a_val;
			if (pt) {
				f = (Fixed*)gf_list_get(l, 0);
				if (f) pt->x = *f;
				f = (Fixed*)gf_list_get(l, 1);
				if (f) pt->y = *f;
				else pt->y = pt->x;
			}

			while (gf_list_count(l)) {
				f = (Fixed*)gf_list_last(l);
				gf_list_rem_last(l);
				gf_free(f);
			}
			gf_list_del(l);
			gf_list_rem(val->values, i);
			if (pt)
				gf_list_insert(val->values, pt, i);
			break;
		default:
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[LASeR] unknown transform type %d\n", transform_type));
			break;
		}
	}
}