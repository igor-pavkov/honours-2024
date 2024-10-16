static void amd_get_topology(struct cpuinfo_x86 *c)
{
	u32 nodes, cores_per_cu = 1;
	u8 node_id;
	int cpu = smp_processor_id();

	/* get information required for multi-node processors */
	if (cpu_has_topoext) {
		u32 eax, ebx, ecx, edx;

		cpuid(0x8000001e, &eax, &ebx, &ecx, &edx);
		nodes = ((ecx >> 8) & 7) + 1;
		node_id = ecx & 7;

		/* get compute unit information */
		smp_num_siblings = ((ebx >> 8) & 3) + 1;
		c->compute_unit_id = ebx & 0xff;
		cores_per_cu += ((ebx >> 8) & 3);
	} else if (cpu_has(c, X86_FEATURE_NODEID_MSR)) {
		u64 value;

		rdmsrl(MSR_FAM10H_NODE_ID, value);
		nodes = ((value >> 3) & 7) + 1;
		node_id = value & 7;
	} else
		return;

	/* fixup multi-node processor information */
	if (nodes > 1) {
		u32 cores_per_node;
		u32 cus_per_node;

		set_cpu_cap(c, X86_FEATURE_AMD_DCM);
		cores_per_node = c->x86_max_cores / nodes;
		cus_per_node = cores_per_node / cores_per_cu;

		/* store NodeID, use llc_shared_map to store sibling info */
		per_cpu(cpu_llc_id, cpu) = node_id;

		/* core id has to be in the [0 .. cores_per_node - 1] range */
		c->cpu_core_id %= cores_per_node;
		c->compute_unit_id %= cus_per_node;
	}
}