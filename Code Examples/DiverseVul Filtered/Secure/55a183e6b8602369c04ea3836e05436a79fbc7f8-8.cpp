GF_Err BD_DecMFFieldList(GF_BifsDecoder * codec, GF_BitStream *bs, GF_Node *node, GF_FieldInfo *field, Bool is_mem_com)
{
	GF_Node *new_node;
	GF_Err e;
	u8 endFlag, qp_local, qp_on, initial_qp;
	GF_ChildNodeItem *last = NULL;
	u32 nbF;

	GF_FieldInfo sffield;

	memset(&sffield, 0, sizeof(GF_FieldInfo));
	sffield.fieldIndex = field->fieldIndex;
	sffield.fieldType = gf_sg_vrml_get_sf_type(field->fieldType);
	sffield.NDTtype = field->NDTtype;
	sffield.name = field->name;

	nbF = 0;
	qp_on = qp_local = 0;
	initial_qp = codec->ActiveQP ? 1 : 0;

	endFlag = gf_bs_read_int(bs, 1);
	while (!endFlag  && (codec->LastError>=0)) {
		if (field->fieldType != GF_SG_VRML_MFNODE) {
			e = gf_sg_vrml_mf_append(field->far_ptr, field->fieldType, & sffield.far_ptr);
			if (e) return e;
			e = gf_bifs_dec_sf_field(codec, bs, node, &sffield, GF_FALSE);
			if (e) return e;
		} else {
			new_node = gf_bifs_dec_node(codec, bs, field->NDTtype);
			//append
			if (new_node) {
				e = gf_node_register(new_node, is_mem_com ? NULL : node);
				if (e) return e;

				//regular coding
				if (node) {
					//special case for QP, register as the current QP
					if (gf_node_get_tag(new_node) == TAG_MPEG4_QuantizationParameter) {
						qp_local = ((M_QuantizationParameter *)new_node)->isLocal;
						//we have a QP in the same scope, remove previous
						if (qp_on) gf_bifs_dec_qp_remove(codec, GF_FALSE);
						e = gf_bifs_dec_qp_set(codec, new_node);
						if (e) return e;
						qp_on = 1;
						if (qp_local) qp_local = 2;
						if (codec->force_keep_qp) {
							e = gf_node_list_add_child_last(field->far_ptr, new_node, &last);
						} else {
							gf_node_register(new_node, NULL);
							gf_node_unregister(new_node, node);
						}
					} else
						//this is generic MFNode container
						e = gf_node_list_add_child_last(field->far_ptr, new_node, &last);

				}
				//proto coding: directly add the child
				else if (codec->pCurrentProto) {
					//TO DO: what happens if this is a QP node on the interface ?
					e = gf_node_list_add_child_last( (GF_ChildNodeItem **)field->far_ptr, new_node, &last);
				}
			} else {
				return codec->LastError;
			}
		}
		if (e) return e;

		endFlag = gf_bs_read_int(bs, 1);

		//according to the spec, the QP applies to the current node itself,
		//not just children. If IsLocal is TRUE remove the node
		if (qp_on && qp_local) {
			if (qp_local == 2) {
				qp_local = 1;
			} else {
				//ask to get rid of QP and reactivate if we had a QP when entering
				gf_bifs_dec_qp_remove(codec, initial_qp);
				qp_local = 0;
				qp_on = 0;
			}
		}
		nbF += 1;
	}
	/*finally delete the QP if any (local or not) as we get out of this node
	and reactivate previous one*/
	if (qp_on) gf_bifs_dec_qp_remove(codec, initial_qp);
	/*this is for QP 14*/
	gf_bifs_dec_qp14_set_length(codec, nbF);
	return GF_OK;
}