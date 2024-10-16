static int uvc_scan_chain(struct uvc_video_device *video)
{
	struct uvc_entity *entity, *prev;
	int id;

	entity = video->oterm;
	uvc_trace(UVC_TRACE_PROBE, "Scanning UVC chain: OT %d", entity->id);
	id = entity->output.bSourceID;
	while (id != 0) {
		prev = entity;
		entity = uvc_entity_by_id(video->dev, id);
		if (entity == NULL) {
			uvc_trace(UVC_TRACE_DESCR, "Found reference to "
				"unknown entity %d.\n", id);
			return -1;
		}

		/* Process entity */
		if (uvc_scan_chain_entity(video, entity) < 0)
			return -1;

		/* Forward scan */
		if (uvc_scan_chain_forward(video, entity, prev) < 0)
			return -1;

		/* Stop when a terminal is found. */
		if (!UVC_ENTITY_IS_UNIT(entity))
			break;

		/* Backward scan */
		id = uvc_scan_chain_backward(video, entity);
		if (id < 0)
			return id;
	}

	/* Initialize the video buffers queue. */
	uvc_queue_init(&video->queue);

	return 0;
}