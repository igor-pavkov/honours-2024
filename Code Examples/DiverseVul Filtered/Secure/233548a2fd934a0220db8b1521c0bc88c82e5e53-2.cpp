static struct uvc_entity *uvc_entity_by_reference(struct uvc_device *dev,
	int id, struct uvc_entity *entity)
{
	unsigned int i;

	if (entity == NULL)
		entity = list_entry(&dev->entities, struct uvc_entity, list);

	list_for_each_entry_continue(entity, &dev->entities, list) {
		switch (UVC_ENTITY_TYPE(entity)) {
		case TT_STREAMING:
			if (entity->output.bSourceID == id)
				return entity;
			break;

		case VC_PROCESSING_UNIT:
			if (entity->processing.bSourceID == id)
				return entity;
			break;

		case VC_SELECTOR_UNIT:
			for (i = 0; i < entity->selector.bNrInPins; ++i)
				if (entity->selector.baSourceID[i] == id)
					return entity;
			break;

		case VC_EXTENSION_UNIT:
			for (i = 0; i < entity->extension.bNrInPins; ++i)
				if (entity->extension.baSourceID[i] == id)
					return entity;
			break;
		}
	}

	return NULL;
}