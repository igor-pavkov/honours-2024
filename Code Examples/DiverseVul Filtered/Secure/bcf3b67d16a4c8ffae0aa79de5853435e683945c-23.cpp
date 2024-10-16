static inline int megasas_alloc_mfi_ctrl_mem(struct megasas_instance *instance)
{
	instance->producer = dma_alloc_coherent(&instance->pdev->dev,
			sizeof(u32), &instance->producer_h, GFP_KERNEL);
	instance->consumer = dma_alloc_coherent(&instance->pdev->dev,
			sizeof(u32), &instance->consumer_h, GFP_KERNEL);

	if (!instance->producer || !instance->consumer) {
		dev_err(&instance->pdev->dev,
			"Failed to allocate memory for producer, consumer\n");
		return -1;
	}

	*instance->producer = 0;
	*instance->consumer = 0;
	return 0;
}