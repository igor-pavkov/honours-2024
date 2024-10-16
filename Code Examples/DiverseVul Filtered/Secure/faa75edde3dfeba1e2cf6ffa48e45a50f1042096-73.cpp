static GF_Node *lsr_read_listener(GF_LASeRCodec *lsr, SVG_Element *parent)
{
	u32 flag;
	GF_FieldInfo info;
	XMLEV_Event *ev = NULL;
	XMLRI *observer, *target, *handler;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_listener);

	observer = target = handler = NULL;

	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	GF_LSR_READ_INT(lsr, flag, 1, "hasDefaultAction");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_XMLEV_ATT_defaultAction, 1, 0, &info);
		GF_LSR_READ_INT(lsr, *(XMLEV_DefaultAction*)info.far_ptr, 1, "defaultAction");
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasEvent");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_XMLEV_ATT_event, 1, 0, &info);
		lsr_read_event_type(lsr, info.far_ptr);
		ev = info.far_ptr;
	}
	/*create default handler but UNINITIALIZED*/
	lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_XMLEV_ATT_handler, 1, 0, &info);
	handler = info.far_ptr;
	GF_LSR_READ_INT(lsr, flag, 1, "hasHandler");
	if (flag) {
		lsr_read_any_uri(lsr, info.far_ptr, "handler");
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasObserver");
	/*TODO double check spec here*/
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_XMLEV_ATT_observer, 1, 0, &info);
		lsr_read_codec_IDREF(lsr, info.far_ptr, "observer");
		observer = info.far_ptr;
	}

	GF_LSR_READ_INT(lsr, flag, 1, "hasPhase");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_XMLEV_ATT_phase, 1, 0, &info);
		GF_LSR_READ_INT(lsr, *(XMLEV_Phase*)info.far_ptr, 1, "phase");
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasPropagate");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_XMLEV_ATT_propagate, 1, 0, &info);
		GF_LSR_READ_INT(lsr, *(XMLEV_Propagate*)info.far_ptr, 1, "propagate");
	}
	GF_LSR_READ_INT(lsr, flag, 1, "hasTarget");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_XMLEV_ATT_target, 1, 0, &info);
		lsr_read_codec_IDREF(lsr, info.far_ptr, "target");
		target = info.far_ptr;
	}

	lsr_read_lsr_enabled(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);

	/*register listener element*/
	{
		Bool post_pone = 0;
		SVG_Element *par = NULL;
		if (observer && observer->type == XMLRI_ELEMENTID) {
			if (observer->target) par = observer->target;
		}
		if (!par && target && (target->type == XMLRI_ELEMENTID)) {
			if (!target->target) post_pone = 1;
			else par = target->target;
		}
		if (!handler->target && !handler->string) {
			handler->type = XMLRI_ELEMENTID;
			handler->target = parent;
		}
		/*FIXME - double check with XML events*/
		if (!par && !observer) {
			/*all non-UI get attched to root*/
			if (ev && (ev->type>GF_EVENT_MOUSEWHEEL)) {
				par = (SVG_Element*) lsr->current_root;
			}
			else if (parent) par = parent;
			else par = (SVG_Element*) lsr->current_root;
		}
		if (!par) post_pone = 1;

		if (post_pone) {
			gf_list_add(lsr->deferred_listeners, elt);
		} else {
			if (!par) par = parent;
			gf_node_dom_listener_add((GF_Node *)par, elt);
		}
	}
	return elt;
}