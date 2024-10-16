static void __heap_add(struct min_heap *heap, struct perf_event *event)
{
	struct perf_event **itrs = heap->data;

	if (event) {
		itrs[heap->nr] = event;
		heap->nr++;
	}
}