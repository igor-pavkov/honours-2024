static void megasas_complete_cmd_dpc(unsigned long instance_addr)
{
	u32 producer;
	u32 consumer;
	u32 context;
	struct megasas_cmd *cmd;
	struct megasas_instance *instance =
				(struct megasas_instance *)instance_addr;
	unsigned long flags;

	/* If we have already declared adapter dead, donot complete cmds */
	if (atomic_read(&instance->adprecovery) == MEGASAS_HW_CRITICAL_ERROR)
		return;

	spin_lock_irqsave(&instance->completion_lock, flags);

	producer = le32_to_cpu(*instance->producer);
	consumer = le32_to_cpu(*instance->consumer);

	while (consumer != producer) {
		context = le32_to_cpu(instance->reply_queue[consumer]);
		if (context >= instance->max_fw_cmds) {
			dev_err(&instance->pdev->dev, "Unexpected context value %x\n",
				context);
			BUG();
		}

		cmd = instance->cmd_list[context];

		megasas_complete_cmd(instance, cmd, DID_OK);

		consumer++;
		if (consumer == (instance->max_fw_cmds + 1)) {
			consumer = 0;
		}
	}

	*instance->consumer = cpu_to_le32(producer);

	spin_unlock_irqrestore(&instance->completion_lock, flags);

	/*
	 * Check if we can restore can_queue
	 */
	megasas_check_and_restore_queue_depth(instance);
}