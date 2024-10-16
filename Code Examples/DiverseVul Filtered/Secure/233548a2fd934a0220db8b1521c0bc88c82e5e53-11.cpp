static int uvc_scan_chain_backward(struct uvc_video_device *video,
	struct uvc_entity *entity)
{
	struct uvc_entity *term;
	int id = -1, i;

	switch (UVC_ENTITY_TYPE(entity)) {
	case VC_EXTENSION_UNIT:
		id = entity->extension.baSourceID[0];
		break;

	case VC_PROCESSING_UNIT:
		id = entity->processing.bSourceID;
		break;

	case VC_SELECTOR_UNIT:
		/* Single-input selector units are ignored. */
		if (entity->selector.bNrInPins == 1) {
			id = entity->selector.baSourceID[0];
			break;
		}

		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(" <- IT");

		video->selector = entity;
		for (i = 0; i < entity->selector.bNrInPins; ++i) {
			id = entity->selector.baSourceID[i];
			term = uvc_entity_by_id(video->dev, id);
			if (term == NULL || !UVC_ENTITY_IS_ITERM(term)) {
				uvc_trace(UVC_TRACE_DESCR, "Selector unit %d "
					"input %d isn't connected to an "
					"input terminal\n", entity->id, i);
				return -1;
			}

			if (uvc_trace_param & UVC_TRACE_PROBE)
				printk(" %d", term->id);

			list_add_tail(&term->chain, &video->iterms);
			uvc_scan_chain_forward(video, term, entity);
		}

		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk("\n");

		id = 0;
		break;
	}

	return id;
}