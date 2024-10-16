const char *__attribute_const__ rdma_event_msg(enum rdma_cm_event_type event)
{
	size_t index = event;

	return (index < ARRAY_SIZE(cma_events) && cma_events[index]) ?
			cma_events[index] : "unrecognized event";
}