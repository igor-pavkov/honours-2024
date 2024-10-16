static inline void megasas_free_ctrl_mem(struct megasas_instance *instance)
{
	kfree(instance->reply_map);
	if (instance->adapter_type == MFI_SERIES) {
		if (instance->producer)
			dma_free_coherent(&instance->pdev->dev, sizeof(u32),
					    instance->producer,
					    instance->producer_h);
		if (instance->consumer)
			dma_free_coherent(&instance->pdev->dev, sizeof(u32),
					    instance->consumer,
					    instance->consumer_h);
	} else {
		megasas_free_fusion_context(instance);
	}
}