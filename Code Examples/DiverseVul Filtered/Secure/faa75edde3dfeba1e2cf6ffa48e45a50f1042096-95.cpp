static void lsr_read_id(GF_LASeRCodec *lsr, GF_Node *n)
{
	GF_FieldInfo info;
	u32 val, id, i, count;
	char *name;
	GF_LSR_READ_INT(lsr, val, 1, "has_id");
	if (!val) return;

	name = NULL;
	id = 1+lsr_read_vluimsbf5(lsr, "ID");
	gf_node_set_id(n, id, name);

	GF_LSR_READ_INT(lsr, val, 1, "reserved");
	/*currently not used*/
	if (val) {
		u32 len = lsr_read_vluimsbf5(lsr, "len");
		GF_LSR_READ_INT(lsr, val, len, "reserved");
	}

	/*update all pending HREFs*/
	count = gf_list_count(lsr->deferred_hrefs);
	for (i=0; i<count; i++) {
		XMLRI *href = (XMLRI *)gf_list_get(lsr->deferred_hrefs, i);
		char *str_id = href ? href->string : NULL;
		if (!str_id) return;
		
		if (str_id[0] == '#') str_id++;
		/*skip 'N'*/
		str_id++;
		if (id == (1 + (u32) atoi(str_id))) {
			href->target = (SVG_Element*) n;
			gf_free(href->string);
			href->string = NULL;
			gf_list_rem(lsr->deferred_hrefs, i);
			i--;
			count--;
		}
	}

	/*update unresolved listeners*/
	count = gf_list_count(lsr->deferred_listeners);
	for (i=0; i<count; i++) {
		GF_Node *par;
		XMLRI *observer = NULL;
		GF_Node *listener = (GF_Node *)gf_list_get(lsr->deferred_listeners, i);

		par = NULL;
		if (gf_node_get_attribute_by_tag(listener, TAG_XMLEV_ATT_observer, GF_FALSE, GF_FALSE, &info) == GF_OK) {
			observer = (XMLRI*)info.far_ptr;
			if (observer->type == XMLRI_ELEMENTID) {
				if (!observer->target) continue;
				else par = (GF_Node*)observer->target;
			}
		}
		if (gf_node_get_attribute_by_tag(listener, TAG_XMLEV_ATT_target, GF_FALSE, GF_FALSE, &info) == GF_OK) {
			if (((XMLRI*)info.far_ptr)->type == XMLRI_ELEMENTID) {
				if (!((XMLRI*)info.far_ptr)->target) continue;
				else if (!par) par = (GF_Node*)((XMLRI*)info.far_ptr)->target;
			}
		}
		/*FIXME - double check with XML events*/
		if (!par && !observer) {
			if (gf_node_get_attribute_by_tag(listener, TAG_XMLEV_ATT_event, GF_FALSE, GF_FALSE, &info) == GF_OK) {
				XMLEV_Event *ev = (XMLEV_Event *)info.far_ptr;
				/*all non-UI get attched to root*/
				if (ev && (ev->type>GF_EVENT_MOUSEWHEEL)) {
					par = (GF_Node*) lsr->current_root;
				}
			}
		}

		assert(par);
		gf_node_dom_listener_add(par, listener);
		gf_list_rem(lsr->deferred_listeners, i);
		i--;
		count--;
	}

	/*update all pending animations*/
	count = gf_list_count(lsr->deferred_anims);
	for (i=0; i<count; i++) {
		SVG_Element *elt = (SVG_Element *)gf_list_get(lsr->deferred_anims, i);
		if (lsr_setup_smil_anim(lsr, elt, NULL)) {
			gf_list_rem(lsr->deferred_anims, i);
			i--;
			count--;
			gf_node_init((GF_Node*)elt);
		}
	}
}