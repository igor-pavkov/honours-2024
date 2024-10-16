static void lsr_read_path_type(GF_LASeRCodec *lsr, GF_Node *n, u32 tag, SVG_PathData *path, const char *name)
{
#if USE_GF_PATH
	GF_Point2D *pt, *ct1, *ct2, *end;
	GF_Point2D orig, ct_orig;
	u32 i, count, cur_pt, type;
	GF_List *pts = gf_list_new();
	lsr_read_point_sequence(lsr, pts, "seq");

	if (!path) {
		GF_FieldInfo info;
		gf_node_get_attribute_by_tag(n, tag, GF_TRUE, 0, &info);
		path = (SVG_PathData*)info.far_ptr;
	} else {
		gf_path_reset(path);
	}
	/*first MoveTo is skipped in LASeR*/
	pt = (GF_Point2D*)gf_list_get(pts, 0);
	if (pt) {
		ct_orig = orig = *pt;
		gf_path_add_move_to_vec(path, pt);
	} else {
		orig.x = orig.y = 0;
		ct_orig = orig;
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CODING, ("[LASeR] Empty path found.\n"));
	}
	cur_pt = 1;
	count = lsr_read_vluimsbf5(lsr, "nbOfTypes");
	for (i=0; i<count; i++) {
		GF_LSR_READ_INT(lsr, type, 5, name);
		switch (type) {
		case LSR_PATH_COM_h:
		case LSR_PATH_COM_l:
		case LSR_PATH_COM_v:
		case LSR_PATH_COM_H:
		case LSR_PATH_COM_V:
		case LSR_PATH_COM_L:
			pt = (GF_Point2D*)gf_list_get(pts, cur_pt);
			if (!pt) goto err_exit;
			gf_path_add_line_to_vec(path, pt);
			cur_pt++;
			break;
		case LSR_PATH_COM_m:
		case LSR_PATH_COM_M:
			pt = (GF_Point2D*)gf_list_get(pts, cur_pt);
			if (!pt) goto err_exit;
			gf_path_add_move_to_vec(path, pt);
			cur_pt++;
			break;
		case LSR_PATH_COM_q:
		case LSR_PATH_COM_Q:
			ct1 = (GF_Point2D*)gf_list_get(pts, cur_pt);
			end = (GF_Point2D*)gf_list_get(pts, cur_pt+1);
			if (!ct1 || !end) goto err_exit;
			gf_path_add_quadratic_to_vec(path, ct1, end);
			orig = *end;
			cur_pt+=2;
			break;
		case LSR_PATH_COM_c:
		case LSR_PATH_COM_C:
			ct1 = (GF_Point2D*)gf_list_get(pts, cur_pt);
			ct2 = (GF_Point2D*)gf_list_get(pts, cur_pt+1);
			end = (GF_Point2D*)gf_list_get(pts, cur_pt+2);
			if (!ct1 || !ct2 || !end) goto err_exit;
			gf_path_add_cubic_to_vec(path, ct1, ct2, end);
			cur_pt+=3;
			ct_orig = *ct2;
			orig = *end;
			break;
		case LSR_PATH_COM_s:
		case LSR_PATH_COM_S:
			ct_orig.x = 2*orig.x - ct_orig.x;
			ct_orig.y = 2*orig.y - ct_orig.y;
			ct2 = (GF_Point2D*)gf_list_get(pts, cur_pt);
			end = (GF_Point2D*)gf_list_get(pts, cur_pt+1);
			if (!ct2 || !end) goto err_exit;
			gf_path_add_cubic_to_vec(path, &ct_orig, ct2, end);
			ct_orig = *ct2;
			orig = *end;
			cur_pt+=2;
			break;
		case LSR_PATH_COM_t:
		case LSR_PATH_COM_T:
			ct_orig.x = 2*orig.x - ct_orig.x;
			ct_orig.y = 2*orig.y - ct_orig.y;
			end = gf_list_get(pts, cur_pt);
			if (!end) goto err_exit;
			gf_path_add_quadratic_to_vec(path, &ct_orig, end);
			orig = *end;
			break;
		case LSR_PATH_COM_z:
		case LSR_PATH_COM_Z:
			gf_path_close(path);
			break;
		}
		if (lsr->last_error) break;
	}
	goto exit;

err_exit:
	lsr->last_error = GF_NON_COMPLIANT_BITSTREAM;

exit:
	while (gf_list_count(pts)) {
		end = (GF_Point2D*)gf_list_get(pts, 0);
		gf_list_rem(pts, 0);
		gf_free(end);
	}
	gf_list_del(pts);

#else
	u32 i, count, c;
	if (!path) {
		lsr->last_error = GF_BAD_PARAM;
		return;
	}
	lsr_read_point_sequence(lsr, path->points, "seq");
	while (gf_list_count(path->commands)) {
		u8 *v = (u8 *)gf_list_last(path->commands);
		gf_list_rem_last(path->commands);
		gf_free(v);
	}

	count = lsr_read_vluimsbf5(lsr, "nbOfTypes");
	for (i=0; i<count; i++) {
		u8 *type = (u8 *)gf_malloc(sizeof(u8));
		if (!type) {
			lsr->last_error = GF_OUT_OF_MEM;
			return;
		}
		GF_LSR_READ_INT(lsr, c, 5, name);

		switch (c) {
		case LSR_PATH_COM_h:
		case LSR_PATH_COM_l:
		case LSR_PATH_COM_v:
		case LSR_PATH_COM_H:
		case LSR_PATH_COM_V:
		case LSR_PATH_COM_L:
			*type=SVG_PATHCOMMAND_L;
			break;
		case LSR_PATH_COM_m:
		case LSR_PATH_COM_M:
			*type=SVG_PATHCOMMAND_M;
			break;
		case LSR_PATH_COM_q:
		case LSR_PATH_COM_Q:
			*type=SVG_PATHCOMMAND_Q;
			break;
		case LSR_PATH_COM_c:
		case LSR_PATH_COM_C:
			*type=SVG_PATHCOMMAND_C;
			break;
		case LSR_PATH_COM_s:
		case LSR_PATH_COM_S:
			*type=SVG_PATHCOMMAND_S;
			break;
		case LSR_PATH_COM_t:
		case LSR_PATH_COM_T:
			*type=SVG_PATHCOMMAND_T;
			break;
		case LSR_PATH_COM_z:
		case LSR_PATH_COM_Z:
			*type=SVG_PATHCOMMAND_Z;
			break;
		}
		gf_list_add(path->commands, type);
	}
#endif
}