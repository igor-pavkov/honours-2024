static int uvc_scan_chain_forward(struct uvc_video_device *video,
	struct uvc_entity *entity, struct uvc_entity *prev)
{
	struct uvc_entity *forward;
	int found;

	/* Forward scan */
	forward = NULL;
	found = 0;

	while (1) {
		forward = uvc_entity_by_reference(video->dev, entity->id,
			forward);
		if (forward == NULL)
			break;

		if (UVC_ENTITY_TYPE(forward) != VC_EXTENSION_UNIT ||
		    forward == prev)
			continue;

		if (forward->extension.bNrInPins != 1) {
			uvc_trace(UVC_TRACE_DESCR, "Extension unit %d has"
				"more than 1 input pin.\n", entity->id);
			return -1;
		}

		list_add_tail(&forward->chain, &video->extensions);
		if (uvc_trace_param & UVC_TRACE_PROBE) {
			if (!found)
				printk(" (-> XU");

			printk(" %d", forward->id);
			found = 1;
		}
	}
	if (found)
		printk(")");

	return 0;
}