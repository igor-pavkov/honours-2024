static inline int vring_need_event(uint16_t event, uint16_t new, uint16_t old)
{
	/* Note: Xen has similar logic for notification hold-off
	 * in include/xen/interface/io/ring.h with req_event and req_prod
	 * corresponding to event_idx + 1 and new respectively.
	 * Note also that req_event and req_prod in Xen start at 1,
	 * event indexes in virtio start at 0. */
	return (uint16_t)(new - event - 1) < (uint16_t)(new - old);
}