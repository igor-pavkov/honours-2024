static irqreturn_t megasas_isr(int irq, void *devp)
{
	struct megasas_irq_context *irq_context = devp;
	struct megasas_instance *instance = irq_context->instance;
	unsigned long flags;
	irqreturn_t rc;

	if (atomic_read(&instance->fw_reset_no_pci_access))
		return IRQ_HANDLED;

	spin_lock_irqsave(&instance->hba_lock, flags);
	rc = megasas_deplete_reply_queue(instance, DID_OK);
	spin_unlock_irqrestore(&instance->hba_lock, flags);

	return rc;
}