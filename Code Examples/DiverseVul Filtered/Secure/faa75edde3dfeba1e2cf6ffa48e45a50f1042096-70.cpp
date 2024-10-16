static void lsr_translate_anim_value(GF_LASeRCodec *lsr, SMIL_AnimateValue *val, u32 coded_type)
{
	switch (val->type) {
	case SVG_StrokeDashArray_datatype:
		//if list of fixed only
		if (coded_type == 8) {
			SVG_StrokeDashArray *da;
			GF_List *l = (GF_List *)val->value;
			u32 i;
			GF_SAFEALLOC(da, SVG_StrokeDashArray);
			if (!da) {
				lsr->last_error = GF_OUT_OF_MEM;
				return;
			}
			da->array.count = gf_list_count(l);
			if (!da->array.count) {
				da->type = SVG_STROKEDASHARRAY_INHERIT;
			} else {
				da->type = SVG_STROKEDASHARRAY_ARRAY;
				da->array.vals = (Fixed *) gf_malloc(sizeof(Fixed)*da->array.count);
				da->array.units = (u8 *) gf_malloc(sizeof(u8)*da->array.count);
				if (!da->array.vals || !da->array.units) {
					lsr->last_error = GF_OUT_OF_MEM;
					return;
				}

				for (i=0; i<da->array.count; i++) {
					Fixed *v = (Fixed *)gf_list_get(l, i);
					da->array.vals[i] = *v;
					da->array.units[i] = 0;
					gf_free(v);
				}
			}
			gf_list_del(l);
			val->value = da;
			return;
		}
		break;
	case SVG_ViewBox_datatype:
		//if list of fixed only
		if (coded_type == 8) {
			SVG_ViewBox *vb;
			GF_List *l = (GF_List *)val->value;
			GF_SAFEALLOC(vb, SVG_ViewBox);
			if (!vb) {
				lsr->last_error = GF_OUT_OF_MEM;
				return;
			}
			if (gf_list_count(l)==4) {
				vb->x = * ((Fixed *)gf_list_get(l, 0));
				vb->y = * ((Fixed *)gf_list_get(l, 1));
				vb->width = * ((Fixed *)gf_list_get(l, 2));
				vb->height = * ((Fixed *)gf_list_get(l, 3));
			}
			while (gf_list_count(l)) {
				Fixed *v = (Fixed *)gf_list_last(l);
				gf_free(v);
				gf_list_rem_last(l);
			}
			gf_list_del(l);
			val->value = vb;
			return;
		}
		break;
	case SVG_Coordinates_datatype:
	{
		SVG_Coordinates *coords;
		if (coded_type==1) {
			GF_List *l = gf_list_new();
			/*allocated value is already an SVG number*/
			gf_list_add(l, val->value);
			coords = (SVG_Coordinates*)gf_malloc(sizeof(SVG_Coordinates));
			if (!coords) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				*coords = l;
				val->value = coords;
			}
			return;
		} else if (coded_type==8) {
			GF_List *l = (GF_List *)val->value;
			u32 i, count = gf_list_count(l);
			for (i=0; i<count; i++) {
				SVG_Coordinate *c;
				Fixed *v = (Fixed *)gf_list_get(l, i);
				c = (SVG_Coordinate*)gf_malloc(sizeof(SVG_Coordinate));
				if (!c) {
					lsr->last_error = GF_OUT_OF_MEM;
				} else {
					c->type = SVG_NUMBER_VALUE;
					c->value = *v;
				}
				gf_free(v);
				gf_list_rem(l, i);
				if (c)
					gf_list_insert(l, c, i);
			}
			coords = (SVG_Coordinates*)gf_malloc(sizeof(SVG_Coordinates));
			if (!coords) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				*coords = (GF_List *) val->value;
				val->value = coords;
			}
			return;
		}
	}
		break;
	case SVG_Motion_datatype:
		if (coded_type==9) {
			GF_Matrix2D *mat;
			SVG_Point *pt = (SVG_Point *)val->value;
			GF_SAFEALLOC(mat, GF_Matrix2D);
			if (mat) {
				gf_mx2d_init(*mat);
				mat->m[2] = pt->x;
				mat->m[5] = pt->y;
				val->value = mat;
			} else {
				lsr->last_error = GF_OUT_OF_MEM;
			}
			gf_free(pt);
			return;
		}
		break;
	default:
		break;
	}
	lsr_delete_anim_value(lsr, val, coded_type);
}