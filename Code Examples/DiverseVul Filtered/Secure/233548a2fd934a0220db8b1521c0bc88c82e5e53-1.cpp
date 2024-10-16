static int uvc_scan_chain_entity(struct uvc_video_device *video,
	struct uvc_entity *entity)
{
	switch (UVC_ENTITY_TYPE(entity)) {
	case VC_EXTENSION_UNIT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(" <- XU %d", entity->id);

		if (entity->extension.bNrInPins != 1) {
			uvc_trace(UVC_TRACE_DESCR, "Extension unit %d has more "
				"than 1 input pin.\n", entity->id);
			return -1;
		}

		list_add_tail(&entity->chain, &video->extensions);
		break;

	case VC_PROCESSING_UNIT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(" <- PU %d", entity->id);

		if (video->processing != NULL) {
			uvc_trace(UVC_TRACE_DESCR, "Found multiple "
				"Processing Units in chain.\n");
			return -1;
		}

		video->processing = entity;
		break;

	case VC_SELECTOR_UNIT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(" <- SU %d", entity->id);

		/* Single-input selector units are ignored. */
		if (entity->selector.bNrInPins == 1)
			break;

		if (video->selector != NULL) {
			uvc_trace(UVC_TRACE_DESCR, "Found multiple Selector "
				"Units in chain.\n");
			return -1;
		}

		video->selector = entity;
		break;

	case ITT_VENDOR_SPECIFIC:
	case ITT_CAMERA:
	case ITT_MEDIA_TRANSPORT_INPUT:
		if (uvc_trace_param & UVC_TRACE_PROBE)
			printk(" <- IT %d\n", entity->id);

		list_add_tail(&entity->chain, &video->iterms);
		break;

	default:
		uvc_trace(UVC_TRACE_DESCR, "Unsupported entity type "
			"0x%04x found in chain.\n", UVC_ENTITY_TYPE(entity));
		return -1;
	}

	return 0;
}