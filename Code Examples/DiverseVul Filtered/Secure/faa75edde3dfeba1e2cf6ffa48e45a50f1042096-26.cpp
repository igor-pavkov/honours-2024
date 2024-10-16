static void lsr_translate_anim_values(GF_LASeRCodec *lsr, SMIL_AnimateValues *val, u32 coded_type)
{
	u32 i, count;
	Bool handled = GF_FALSE;
	GF_List *list, *new_list;

	list = val->values;
	switch (val->type) {
	case SVG_StrokeDashArray_datatype:
		if (coded_type == 8) handled = GF_TRUE;
		break;
	case SVG_ViewBox_datatype:
		if (coded_type == 8) handled = GF_TRUE;
		break;
	case SVG_Coordinates_datatype:
		if (coded_type == 8) handled = GF_TRUE;
		break;
	case SVG_Motion_datatype:
		if (coded_type==9) handled = GF_TRUE;
		break;
	default:
		break;
	}
	if (!handled) {
		while (gf_list_count(list)) {
			SMIL_AnimateValue a_val;
			a_val.type = 0;
			a_val.value = gf_list_pop_back(list);
			lsr_delete_anim_value(lsr, &a_val, coded_type);
		}
		gf_list_del(list);
		val->values = gf_list_new();
		return;
	}

	val->values = new_list = gf_list_new();
	count = gf_list_count(list);
	for (i=0; i<count; i++) {
		switch (val->type) {
		case SVG_StrokeDashArray_datatype:
		{
			SVG_StrokeDashArray *da;
			GF_List *l = (GF_List *)gf_list_get(list, i);
			u32 j;
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
				da->array.vals = (Fixed *)gf_malloc(sizeof(Fixed)*da->array.count);
				da->array.units = (u8 *) gf_malloc(sizeof(u8)*da->array.count);
				if (!da->array.vals || !da->array.units) {
					lsr->last_error = GF_OUT_OF_MEM;
					return;
				}
				for (j=0; j<da->array.count; j++) {
					Fixed *v = (Fixed *)gf_list_get(l, j);
					da->array.vals[j] = *v;
					da->array.units[j] = 0;
					gf_free(v);
				}
			}
			gf_list_del(l);
			gf_list_add(new_list, da);
		}
		break;
		case SVG_ViewBox_datatype:
		{
			SVG_ViewBox *vb;
			GF_List *l = (GF_List *)gf_list_get(list, i);
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
				Fixed *v=(Fixed *)gf_list_last(l);
				gf_free(v);
				gf_list_rem_last(l);
			}
			gf_list_del(l);
			gf_list_add(new_list, vb);
		}
		break;
		case SVG_Coordinates_datatype:
		{
			SVG_Coordinates *coords;
			GF_List *l = (GF_List *)gf_list_get(list, i);
			u32 j, count2;
			count2 = gf_list_count(l);
			for (j=0; j<count2; j++) {
				Fixed *v = (Fixed *)gf_list_get(l, j);
				SVG_Coordinate *c = (SVG_Coordinate *)gf_malloc(sizeof(SVG_Coordinate));
				if (!c) {
					lsr->last_error = GF_OUT_OF_MEM;
				} else {
					c->type = SVG_NUMBER_VALUE;
					c->value = *v;
				}
				gf_list_rem(l, j);
				gf_free(v);
				if (c)
					gf_list_insert(l, c, j);
			}

			coords = (SVG_Coordinates*)gf_malloc(sizeof(SVG_Coordinates));
			if (!coords) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				*coords = l;
				gf_list_add(new_list, coords);
			}
		}
		break;

		case SVG_Motion_datatype:
 		{
			GF_Point2D *pt = (GF_Point2D *)gf_list_get(list, i);
			GF_Matrix2D *m = (GF_Matrix2D *)gf_malloc(sizeof(GF_Matrix2D ));
			if (!m) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				gf_mx2d_init(*m);
				m->m[2] = pt->x;
				m->m[5] = pt->y;
				gf_list_add(new_list, m);
			}
			gf_free(pt);
		}
			break;
		}
	}
	gf_list_del(list);
}