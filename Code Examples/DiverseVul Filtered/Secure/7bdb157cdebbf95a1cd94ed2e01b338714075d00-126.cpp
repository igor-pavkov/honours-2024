static void __perf_event_header_size(struct perf_event *event, u64 sample_type)
{
	struct perf_sample_data *data;
	u16 size = 0;

	if (sample_type & PERF_SAMPLE_IP)
		size += sizeof(data->ip);

	if (sample_type & PERF_SAMPLE_ADDR)
		size += sizeof(data->addr);

	if (sample_type & PERF_SAMPLE_PERIOD)
		size += sizeof(data->period);

	if (sample_type & PERF_SAMPLE_WEIGHT)
		size += sizeof(data->weight);

	if (sample_type & PERF_SAMPLE_READ)
		size += event->read_size;

	if (sample_type & PERF_SAMPLE_DATA_SRC)
		size += sizeof(data->data_src.val);

	if (sample_type & PERF_SAMPLE_TRANSACTION)
		size += sizeof(data->txn);

	if (sample_type & PERF_SAMPLE_PHYS_ADDR)
		size += sizeof(data->phys_addr);

	if (sample_type & PERF_SAMPLE_CGROUP)
		size += sizeof(data->cgroup);

	event->header_size = size;
}