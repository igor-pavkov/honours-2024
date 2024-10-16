static void do_print_node(GF_Node *node, GF_SceneGraph *sg, const char *name, u32 graph_type, Bool is_nodefield, Bool do_cov)
{
	u32 nbF, i;
	GF_FieldInfo f;
#ifndef GPAC_DISABLE_BIFS
	u8 qt, at;
	Fixed bmin, bmax;
	u32 nbBits;
#endif /*GPAC_DISABLE_BIFS*/

	nbF = gf_node_get_field_count(node);

	if (is_nodefield) {
		char szField[1024];
		u32 tfirst, tlast;
		if (gf_node_get_field_by_name(node, szField, &f) != GF_OK) {
			fprintf(stderr, "Field %s is not a member of node %s\n", szField, name);
			return;
		}
		fprintf(stderr, "Allowed nodes in %s.%s:\n", name, szField);
		if (graph_type==1) {
			tfirst = GF_NODE_RANGE_FIRST_X3D;
			tlast = GF_NODE_RANGE_LAST_X3D;
		} else {
			tfirst = GF_NODE_RANGE_FIRST_MPEG4;
			tlast = GF_NODE_RANGE_LAST_MPEG4;
		}
		for (i=tfirst; i<tlast; i++) {
			GF_Node *tmp = gf_node_new(sg, i);
			gf_node_register(tmp, NULL);
			if (gf_node_in_table_by_tag(i, f.NDTtype)) {
				const char *nname = gf_node_get_class_name(tmp);
				if (nname && strcmp(nname, "Unknown Node")) {
					fprintf(stderr, "\t%s\n", nname);
				}
			}
			gf_node_unregister(tmp, NULL);
		}
		return;
	}
	if (do_cov) {
		u32 ndt;
		if (graph_type==0) {
			u32 all;
			gf_node_mpeg4_type_by_class_name(name);
			gf_bifs_get_child_table(node);
			all = gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_ALL);
			for (i=0; i<all; i++) {
				u32 res;
				gf_sg_script_get_field_index(node, i, GF_SG_FIELD_CODING_ALL, &res);
			}

			gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_DEF);
			gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_IN);
			gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_OUT);
			gf_node_get_num_fields_in_mode(node, GF_SG_FIELD_CODING_DYN);
		}
		else if (graph_type==1) gf_node_x3d_type_by_class_name(name);
		for (ndt=NDT_SFWorldNode; ndt<NDT_LAST; ndt++) {
			gf_node_in_table_by_tag(gf_node_get_tag(node), ndt);
		}
	}
	fprintf(stderr, "%s {\n", name);

	for (i=0; i<nbF; i++) {
		gf_node_get_field(node, i, &f);
		if (graph_type==2) {
			fprintf(stderr, "\t%s=\"...\"\n", f.name);
			continue;
		}

		fprintf(stderr, "\t%s %s %s", gf_sg_vrml_get_event_type_name(f.eventType, 0), gf_sg_vrml_get_field_type_name(f.fieldType), f.name);
		if (f.fieldType==GF_SG_VRML_SFNODE) fprintf(stderr, " NULL");
		else if (f.fieldType==GF_SG_VRML_MFNODE) fprintf(stderr, " []");
		else if (gf_sg_vrml_is_sf_field(f.fieldType)) {
			fprintf(stderr, " ");
			PrintNodeSFField(f.fieldType, f.far_ptr);
		} else {
			void *ptr;
			u32 j, sftype;
			GenMFField *mffield = (GenMFField *) f.far_ptr;
			fprintf(stderr, " [");
			sftype = gf_sg_vrml_get_sf_type(f.fieldType);
			for (j=0; j<mffield->count; j++) {
				if (j) fprintf(stderr, " ");
				gf_sg_vrml_mf_get_item(f.far_ptr, f.fieldType, &ptr, j);
				PrintNodeSFField(sftype, ptr);
			}
			fprintf(stderr, "]");
		}
#ifndef GPAC_DISABLE_BIFS
		if (gf_bifs_get_aq_info(node, i, &qt, &at, &bmin, &bmax, &nbBits)) {
			if (qt) {
				fprintf(stderr, " #QP=%d", qt);
				if (qt==13) fprintf(stderr, " NbBits=%d", nbBits);
				if (bmin && bmax) {
					fprintf(stderr, " Bounds=[");
					PrintFixed(bmin, 0);
					fprintf(stderr, ",");
					PrintFixed(bmax, 0);
					fprintf(stderr, "]");
				}
			}
		}
#endif /*GPAC_DISABLE_BIFS*/
		fprintf(stderr, "\n");

		if (do_cov) {
			gf_node_get_field_by_name(node, (char *) f.name, &f);
		}
	}
	fprintf(stderr, "}\n\n");

}