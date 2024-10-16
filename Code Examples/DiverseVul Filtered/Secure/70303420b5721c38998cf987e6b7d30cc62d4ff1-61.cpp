static int filter_pred_cpu(struct filter_pred *pred, void *event)
{
	int cpu, cmp;

	cpu = raw_smp_processor_id();
	cmp = pred->val;

	switch (pred->op) {
	case OP_EQ:
		return cpu == cmp;
	case OP_NE:
		return cpu != cmp;
	case OP_LT:
		return cpu < cmp;
	case OP_LE:
		return cpu <= cmp;
	case OP_GT:
		return cpu > cmp;
	case OP_GE:
		return cpu >= cmp;
	default:
		return 0;
	}
}