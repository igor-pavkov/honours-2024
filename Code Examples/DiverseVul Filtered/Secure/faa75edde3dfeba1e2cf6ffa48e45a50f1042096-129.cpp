static GF_Node *lsr_read_update_content_model(GF_LASeRCodec *lsr, SVG_Element *parent)
{
	u32 flag;
	GF_Node *n=NULL;
	GF_LSR_READ_INT(lsr, flag, 1, "ch4");
	if (flag) {
		GF_LSR_READ_INT(lsr, flag, 3, "ch61");
		switch (flag) {
		case LSR_UPDATE_CONTENT_MODEL2_conditional:
			n = lsr_read_conditional(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL2_cursorManager:
			n = lsr_read_cursorManager(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL2_extend:
			lsr_read_extend_class(lsr, NULL, 0, "extend");
			return NULL;
		case LSR_UPDATE_CONTENT_MODEL2_private:
			lsr_read_private_element_container(lsr);
			return NULL;
		case LSR_UPDATE_CONTENT_MODEL2_rectClip:
			n = lsr_read_rectClip(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL2_simpleLayout:
			n = lsr_read_simpleLayout(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL2_selector:
			n = lsr_read_selector(lsr);
			break;
		}
	} else {
		GF_LSR_READ_INT(lsr, flag, 6, "ch6");
		switch(flag) {
		case LSR_UPDATE_CONTENT_MODEL_a:
			n = lsr_read_a(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_animate:
			n = lsr_read_animate(lsr, parent, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_animateColor:
			n = lsr_read_animate(lsr, parent, 1);
			break;
		case LSR_UPDATE_CONTENT_MODEL_animateMotion:
			n = lsr_read_animateMotion(lsr, parent);
			break;
		case LSR_UPDATE_CONTENT_MODEL_animateTransform:
			n = lsr_read_animateTransform(lsr, parent);
			break;
		case LSR_UPDATE_CONTENT_MODEL_audio:
			n = lsr_read_audio(lsr, parent);
			break;
		case LSR_UPDATE_CONTENT_MODEL_circle:
			n = lsr_read_circle(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_defs:
			n = lsr_read_defs(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_desc:
			n = lsr_read_data(lsr, TAG_SVG_desc);
			break;
		case LSR_UPDATE_CONTENT_MODEL_ellipse:
			n = lsr_read_ellipse(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_foreignObject:
			n = lsr_read_foreignObject(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_g:
			n = lsr_read_g(lsr, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_image:
			n = lsr_read_image(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_line:
			n = lsr_read_line(lsr, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_linearGradient:
			n = lsr_read_linearGradient(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_metadata:
			n = lsr_read_data(lsr, TAG_SVG_metadata);
			break;
		case LSR_UPDATE_CONTENT_MODEL_mpath:
			n = lsr_read_mpath(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_path:
			n = lsr_read_path(lsr, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_polygon:
			n = lsr_read_polygon(lsr, 0, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_polyline:
			n = lsr_read_polygon(lsr, 1, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_radialGradient:
			n = lsr_read_radialGradient(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_rect:
			n = lsr_read_rect(lsr, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_script:
			n = lsr_read_script(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_set:
			n = lsr_read_set(lsr, parent);
			break;
		case LSR_UPDATE_CONTENT_MODEL_stop:
			n = lsr_read_stop(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_svg:
			n = lsr_read_svg(lsr, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_switch:
			n = lsr_read_switch(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_text:
			n = lsr_read_text(lsr, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_title:
			n = lsr_read_data(lsr, TAG_SVG_title);
			break;
		case LSR_UPDATE_CONTENT_MODEL_tspan:
			n = lsr_read_tspan(lsr);
			break;
		case LSR_UPDATE_CONTENT_MODEL_use:
			n = lsr_read_use(lsr, 0);
			break;
		case LSR_UPDATE_CONTENT_MODEL_video:
			n = lsr_read_video(lsr, parent);
			break;
		case LSR_UPDATE_CONTENT_MODEL_listener:
			n = lsr_read_listener(lsr, parent);
			break;
		}
	}
	if (n && n->sgprivate->interact && n->sgprivate->interact->dom_evt) {
		GF_DOM_Event evt;
		memset(&evt, 0, sizeof(GF_DOM_Event));
		evt.type = GF_EVENT_LOAD;
		gf_dom_event_fire(n, &evt);
	}
	return n;
}