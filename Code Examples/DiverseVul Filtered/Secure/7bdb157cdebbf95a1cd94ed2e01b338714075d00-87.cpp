static void perf_event_mmap_output(struct perf_event *event,
				   void *data)
{
	struct perf_mmap_event *mmap_event = data;
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	int size = mmap_event->event_id.header.size;
	u32 type = mmap_event->event_id.header.type;
	int ret;

	if (!perf_event_mmap_match(event, data))
		return;

	if (event->attr.mmap2) {
		mmap_event->event_id.header.type = PERF_RECORD_MMAP2;
		mmap_event->event_id.header.size += sizeof(mmap_event->maj);
		mmap_event->event_id.header.size += sizeof(mmap_event->min);
		mmap_event->event_id.header.size += sizeof(mmap_event->ino);
		mmap_event->event_id.header.size += sizeof(mmap_event->ino_generation);
		mmap_event->event_id.header.size += sizeof(mmap_event->prot);
		mmap_event->event_id.header.size += sizeof(mmap_event->flags);
	}

	perf_event_header__init_id(&mmap_event->event_id.header, &sample, event);
	ret = perf_output_begin(&handle, event,
				mmap_event->event_id.header.size);
	if (ret)
		goto out;

	mmap_event->event_id.pid = perf_event_pid(event, current);
	mmap_event->event_id.tid = perf_event_tid(event, current);

	perf_output_put(&handle, mmap_event->event_id);

	if (event->attr.mmap2) {
		perf_output_put(&handle, mmap_event->maj);
		perf_output_put(&handle, mmap_event->min);
		perf_output_put(&handle, mmap_event->ino);
		perf_output_put(&handle, mmap_event->ino_generation);
		perf_output_put(&handle, mmap_event->prot);
		perf_output_put(&handle, mmap_event->flags);
	}

	__output_copy(&handle, mmap_event->file_name,
				   mmap_event->file_size);

	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
out:
	mmap_event->event_id.header.size = size;
	mmap_event->event_id.header.type = type;
}