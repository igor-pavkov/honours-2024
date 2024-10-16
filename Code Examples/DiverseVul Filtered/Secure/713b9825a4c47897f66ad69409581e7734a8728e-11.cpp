static bool io_wq_worker_affinity(struct io_worker *worker, void *data)
{
	struct online_data *od = data;

	if (od->online)
		cpumask_set_cpu(od->cpu, worker->wqe->cpu_mask);
	else
		cpumask_clear_cpu(od->cpu, worker->wqe->cpu_mask);
	return false;
}