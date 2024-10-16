static GF_Err lsr_read_command_list(GF_LASeRCodec *lsr, GF_List *com_list, SVG_Element *cond, Bool first_imp)
{
	GF_Node *n;
	GF_Command *com;
	u32 i, type, count;

	if (cond) {
		u32 s_len;
		GF_DOMUpdates *up = gf_dom_add_updates_node((GF_Node*)cond);
		gf_node_set_callback_function((GF_Node*)up, lsr_exec_command_list);
		gf_node_set_private((GF_Node *) up, lsr);

		s_len = lsr_read_vluimsbf5(lsr, NULL/*"encoding-length" - don't log to avoid corrupting the log order!!*/);
		gf_bs_align(lsr->bs);
		/*not in memory mode, direct decode*/
		if (!lsr->memory_dec) {
			com_list = NULL;
			up->data_size = s_len;
			up->data = (char*)gf_malloc(sizeof(char)*s_len);
			if (!up->data) {
				lsr->last_error = GF_OUT_OF_MEM;
			} else {
				gf_bs_read_data(lsr->bs, up->data, s_len);
			}
			goto exit;
		}
		/*memory mode, decode commands*/
		else {
			com_list = up->updates;
		}
	}
	count = lsr_read_vluimsbf5(lsr, "occ0");
	if (first_imp) count++;

	for (i=0; i<count; i++) {
		GF_LSR_READ_INT(lsr, type, 4, "ch4");
		switch (type) {
		case LSR_UPDATE_ADD:
		case LSR_UPDATE_INSERT:
		case LSR_UPDATE_REPLACE:
			lsr_read_add_replace_insert(lsr, com_list, type);
			break;
		case LSR_UPDATE_DELETE:
			lsr_read_delete(lsr, com_list);
			break;
		case LSR_UPDATE_NEW_SCENE:
		case LSR_UPDATE_REFRESH_SCENE: /*TODO FIXME, depends on decoder state*/
			if (type==5) lsr_read_vluimsbf5(lsr, "time");
			lsr_read_any_attribute(lsr, NULL, 1);
			if (com_list) {
				n = lsr_read_svg(lsr, 0);
				if (!n)
					return (lsr->last_error = GF_NON_COMPLIANT_BITSTREAM);
				gf_node_register(n, NULL);
				com = gf_sg_command_new(lsr->sg, (type==5) ? GF_SG_LSR_REFRESH_SCENE : GF_SG_LSR_NEW_SCENE);
				com->node = n;
				gf_list_add(com_list, com);
			} else {
				gf_sg_reset(lsr->sg);
				gf_sg_set_scene_size_info(lsr->sg, 0, 0, 1);
				n = lsr_read_svg(lsr, 1);
				if (!n)
					return (lsr->last_error = GF_NON_COMPLIANT_BITSTREAM);
			}
			break;
		case LSR_UPDATE_TEXT_CONTENT:
			lsr_read_byte_align_string(lsr, NULL, "textContent");
			break;
		case LSR_UPDATE_SEND_EVENT:
			lsr_read_send_event(lsr, com_list);
			break;
		case LSR_UPDATE_RESTORE:
			lsr_read_restore(lsr, com_list);
			break;
		case LSR_UPDATE_SAVE:
			lsr_read_save(lsr, com_list);
			break;
		case LSR_UPDATE_EXTEND:
		{
			u32 extID;
			GF_LSR_READ_INT(lsr, extID, lsr->info->cfg.extensionIDBits, "extensionID");
			/*len = */lsr_read_vluimsbf5(lsr, "len");
			if (extID==2) {
				u32 j, sub_count;
				lsr_read_vluimsbf5(lsr, "reserved");
				sub_count = lsr_read_vluimsbf5(lsr, "occ2");
				for (j=0; j<sub_count; j++) {
					u32 k, occ3;
					GF_LSR_READ_INT(lsr, k, 2, "reserved");
					occ3 = lsr_read_vluimsbf5(lsr, "occ3");
					for (k=0; k<occ3; k++) {
						u32 sub_type, idref;
						GF_LSR_READ_INT(lsr, sub_type, 2, "ch5");
						switch (sub_type) {
						case 1:
						case 2:
							idref = lsr_read_codec_IDREF_command(lsr, "ref");

							n = gf_sg_find_node(lsr->sg, idref);
							if (com_list) {
								com = gf_sg_command_new(lsr->sg, (sub_type==1) ? GF_SG_LSR_ACTIVATE : GF_SG_LSR_DEACTIVATE);
								if (n) {
									com->node = n;
									gf_node_register(n, NULL);
								} else {
									com->RouteID = idref;
								}
								gf_list_add(com_list, com);
							} else if (sub_type==1) {
								if (!n) return GF_NON_COMPLIANT_BITSTREAM;
								gf_node_activate(n);
							} else {
								if (!n) return GF_NON_COMPLIANT_BITSTREAM;
								gf_node_deactivate(n);
							}
							break;
						default:
							lsr_read_private_element_container(lsr);
							break;
						}
						if (lsr->last_error) return lsr->last_error;
					}
				}
			}
		}
		break;
		default:
			return (lsr->last_error = GF_NON_COMPLIANT_BITSTREAM);
		}
		/*same-coding scope is command-based (to check in the spec)*/
		if (cond) {
			lsr->prev_g = NULL;
			lsr->prev_line = NULL;
			lsr->prev_path = NULL;
			lsr->prev_polygon = NULL;
			lsr->prev_rect = NULL;
			lsr->prev_text = NULL;
			lsr->prev_use = NULL;
		}

		if (lsr->last_error)
			return lsr->last_error;
	}
exit:
	/*script is align*/
	if (cond) {
		gf_bs_align(lsr->bs);
		lsr_read_object_content(lsr, (SVG_Element*)cond);
		lsr->prev_g = NULL;
		lsr->prev_line = NULL;
		lsr->prev_path = NULL;
		lsr->prev_polygon = NULL;
		lsr->prev_rect = NULL;
		lsr->prev_text = NULL;
		lsr->prev_use = NULL;
	}
	return GF_OK;
}