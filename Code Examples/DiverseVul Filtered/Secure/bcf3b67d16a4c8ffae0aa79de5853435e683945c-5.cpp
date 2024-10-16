static void megasas_setup_reply_map(struct megasas_instance *instance)
{
	const struct cpumask *mask;
	unsigned int queue, cpu;

	for (queue = 0; queue < instance->msix_vectors; queue++) {
		mask = pci_irq_get_affinity(instance->pdev, queue);
		if (!mask)
			goto fallback;

		for_each_cpu(cpu, mask)
			instance->reply_map[cpu] = queue;
	}
	return;

fallback:
	for_each_possible_cpu(cpu)
		instance->reply_map[cpu] = cpu % instance->msix_vectors;
}