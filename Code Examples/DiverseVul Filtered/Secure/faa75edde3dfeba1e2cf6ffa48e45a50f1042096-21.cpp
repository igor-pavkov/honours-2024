static void lsr_read_event_type(GF_LASeRCodec *lsr, XMLEV_Event *evtType)
{
	u32 flag;
	memset(evtType, 0, sizeof(XMLEV_Event));

	GF_LSR_READ_INT(lsr, flag, 1, "choice");
	if (!flag) {
		char *evtName, *sep;
		evtName = NULL;
		lsr_read_byte_align_string(lsr, &evtName, "evtString");
		evtType->type = evtType->parameter = 0;
		if (evtName) {
			sep = strchr(evtName, '(');
			if (sep) {
				char *param;
				sep[0] = 0;
				evtType->type = gf_dom_event_type_by_name(evtName);
				sep[0] = '(';
				param = sep+1;
				sep = strchr(evtName, ')');
				if (sep) sep[0]=0;
				if (evtType->type==GF_EVENT_REPEAT) {
					evtType->parameter = atoi(param);
				} else {
					evtType->parameter = gf_dom_get_key_type(param);
				}
			} else {
				evtType->type = gf_dom_event_type_by_name(evtName);
			}
			gf_free(evtName);
		}
	} else {
		evtType->parameter = 0;
		GF_LSR_READ_INT(lsr, flag, 6, "event");
		switch (flag) {
		case LSR_EVT_abort:
			evtType->type = GF_EVENT_ABORT;
			break;
		case LSR_EVT_accessKey:
			evtType->type = GF_EVENT_KEYDOWN;
			break;
		case LSR_EVT_activate:
			evtType->type = GF_EVENT_ACTIVATE;
			break;
		case LSR_EVT_activatedEvent:
			evtType->type = GF_EVENT_ACTIVATED;
			break;
		case LSR_EVT_beginEvent:
			evtType->type = GF_EVENT_BEGIN_EVENT;
			break;
		case LSR_EVT_click:
			evtType->type = GF_EVENT_CLICK;
			break;
		case LSR_EVT_deactivatedEvent:
			evtType->type = GF_EVENT_DEACTIVATED;
			break;
		case LSR_EVT_endEvent:
			evtType->type = GF_EVENT_END_EVENT;
			break;
		case LSR_EVT_error:
			evtType->type = GF_EVENT_ERROR;
			break;
		case LSR_EVT_executionTime:
			evtType->type = GF_EVENT_EXECUTION_TIME;
			break;
		case LSR_EVT_focusin:
			evtType->type = GF_EVENT_FOCUSIN;
			break;
		case LSR_EVT_focusout:
			evtType->type = GF_EVENT_FOCUSOUT;
			break;
		case LSR_EVT_keydown:
			evtType->type = GF_EVENT_KEYDOWN;
			break;
		case LSR_EVT_keyup:
			evtType->type = GF_EVENT_KEYUP;
			break;
		case LSR_EVT_load:
			evtType->type = GF_EVENT_LOAD;
			break;
		case LSR_EVT_longAccessKey:
			evtType->type = GF_EVENT_LONGKEYPRESS;
			break;
		case LSR_EVT_mousedown:
			evtType->type = GF_EVENT_MOUSEDOWN;
			break;
		case LSR_EVT_mousemove:
			evtType->type = GF_EVENT_MOUSEMOVE;
			break;
		case LSR_EVT_mouseout:
			evtType->type = GF_EVENT_MOUSEOUT;
			break;
		case LSR_EVT_mouseover:
			evtType->type = GF_EVENT_MOUSEOVER;
			break;
		case LSR_EVT_mouseup:
			evtType->type = GF_EVENT_MOUSEUP;
			break;
		case LSR_EVT_pause:
			evtType->type = GF_EVENT_PAUSE;
			break;
		case LSR_EVT_pausedEvent:
			evtType->type = GF_EVENT_PAUSED_EVENT;
			break;
		case LSR_EVT_play:
			evtType->type = GF_EVENT_PLAY;
			break;
		case LSR_EVT_repeatEvent:
			evtType->type = GF_EVENT_REPEAT_EVENT;
			break;
		case LSR_EVT_repeatKey:
			evtType->type = GF_EVENT_REPEAT_KEY;
			break;
		case LSR_EVT_resize:
			evtType->type = GF_EVENT_RESIZE;
			break;
		case LSR_EVT_resumedEvent:
			evtType->type = GF_EVENT_RESUME_EVENT;
			break;
		case LSR_EVT_scroll:
			evtType->type = GF_EVENT_SCROLL;
			break;
		case LSR_EVT_shortAccessKey:
			evtType->type = GF_EVENT_SHORT_ACCESSKEY;
			break;
		case LSR_EVT_textinput:
			evtType->type = GF_EVENT_TEXTINPUT;
			break;
		case LSR_EVT_unload:
			evtType->type = GF_EVENT_UNLOAD;
			break;
		case LSR_EVT_zoom:
			evtType->type = GF_EVENT_ZOOM;
			break;
		default:
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[LASeR] Undefined LASeR event %d\n", flag));
			break;
		}
		switch (flag) {
		case LSR_EVT_accessKey:
		case LSR_EVT_longAccessKey:
		case LSR_EVT_repeatKey:
		case LSR_EVT_shortAccessKey:
			evtType->parameter = lsr_read_vluimsbf5(lsr, "keyCode");
			evtType->parameter  = lsr_to_dom_key(evtType->parameter);
			break;
		}
	}
}