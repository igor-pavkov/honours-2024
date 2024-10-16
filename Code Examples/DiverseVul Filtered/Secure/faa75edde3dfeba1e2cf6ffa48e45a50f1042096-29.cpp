static GF_Err lsr_read_send_event(GF_LASeRCodec *lsr, GF_List *com_list)
{
	u32 flag, idref;
	s32 detail;
	SVG_Number x, y;
	XMLEV_Event event;
	lsr_read_event_type(lsr, &event);

	detail = 0;
	GF_LSR_READ_INT(lsr, flag, 1, "has_intvalue");
	if (flag) {
		GF_LSR_READ_INT(lsr, flag, 1, "sign");
		detail = lsr_read_vluimsbf5(lsr, "value");
		if (flag) detail = -detail;

		switch (event.type) {
		case GF_EVENT_KEYDOWN:
		case GF_EVENT_LONGKEYPRESS:
		case GF_EVENT_REPEAT_KEY:
		case GF_EVENT_SHORT_ACCESSKEY:
			detail = lsr_to_dom_key(detail);
			break;
		default:
			break;
		}
	}
	x.value = y.value = 0;
	GF_LSR_READ_INT(lsr, flag, 1, "has_pointvalue");
	if (flag) {
		lsr_read_coordinate(lsr, &x, 0, "x");
		lsr_read_coordinate(lsr, &y, 0, "y");
	}
	idref = lsr_read_codec_IDREF_command(lsr, "idref");

	GF_LSR_READ_INT(lsr, flag, 1, "has_pointvalue");
	if (flag) {
		lsr_read_byte_align_string(lsr, NULL, "string");
	}
	lsr_read_any_attribute(lsr, NULL, 1);

	if (!com_list) {
		GF_DOM_Event evt;
		GF_Node *target = gf_sg_find_node(lsr->sg, idref);
		if (!target) return GF_OK;

		memset(&evt, 0, sizeof(GF_DOM_Event));
		evt.type = event.type;
		evt.detail = detail ? detail : (s32) event.parameter;
		evt.clientX = FIX2INT(x.value);
		evt.clientY = FIX2INT(y.value);
		gf_dom_event_fire(target, &evt);

	} else {
		GF_Command *com = gf_sg_command_new(lsr->sg, GF_SG_LSR_SEND_EVENT);
		gf_list_add(com_list, com);
		com->node = gf_sg_find_node(lsr->sg, idref);
		if (!com->node) {
			com->RouteID = idref;
			gf_list_add(lsr->unresolved_commands, com);
		} else {
			gf_node_register(com->node, NULL);
		}
		com->send_event_integer = detail ? detail : (s32) event.parameter;
		com->send_event_name = event.type;
		com->send_event_x = FIX2INT(x.value);
		com->send_event_y = FIX2INT(y.value);
	}
	return GF_OK;
}