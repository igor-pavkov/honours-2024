struct megasas_cmd *megasas_get_cmd(struct megasas_instance
						  *instance)
{
	unsigned long flags;
	struct megasas_cmd *cmd = NULL;

	spin_lock_irqsave(&instance->mfi_pool_lock, flags);

	if (!list_empty(&instance->cmd_pool)) {
		cmd = list_entry((&instance->cmd_pool)->next,
				 struct megasas_cmd, list);
		list_del_init(&cmd->list);
	} else {
		dev_err(&instance->pdev->dev, "Command pool empty!\n");
	}

	spin_unlock_irqrestore(&instance->mfi_pool_lock, flags);
	return cmd;
}