static int megasas_alloc_ctrl_mem(struct megasas_instance *instance)
{
	instance->reply_map = kcalloc(nr_cpu_ids, sizeof(unsigned int),
				      GFP_KERNEL);
	if (!instance->reply_map)
		return -ENOMEM;

	switch (instance->adapter_type) {
	case MFI_SERIES:
		if (megasas_alloc_mfi_ctrl_mem(instance))
			goto fail;
		break;
	case AERO_SERIES:
	case VENTURA_SERIES:
	case THUNDERBOLT_SERIES:
	case INVADER_SERIES:
		if (megasas_alloc_fusion_context(instance))
			goto fail;
		break;
	}

	return 0;
 fail:
	kfree(instance->reply_map);
	instance->reply_map = NULL;
	return -ENOMEM;
}