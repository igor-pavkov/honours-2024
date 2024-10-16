static void fm10k_update_itr(struct fm10k_ring_container *ring_container)
{
	unsigned int avg_wire_size, packets, itr_round;

	/* Only update ITR if we are using adaptive setting */
	if (!ITR_IS_ADAPTIVE(ring_container->itr))
		goto clear_counts;

	packets = ring_container->total_packets;
	if (!packets)
		goto clear_counts;

	avg_wire_size = ring_container->total_bytes / packets;

	/* The following is a crude approximation of:
	 *  wmem_default / (size + overhead) = desired_pkts_per_int
	 *  rate / bits_per_byte / (size + ethernet overhead) = pkt_rate
	 *  (desired_pkt_rate / pkt_rate) * usecs_per_sec = ITR value
	 *
	 * Assuming wmem_default is 212992 and overhead is 640 bytes per
	 * packet, (256 skb, 64 headroom, 320 shared info), we can reduce the
	 * formula down to
	 *
	 *  (34 * (size + 24)) / (size + 640) = ITR
	 *
	 * We first do some math on the packet size and then finally bitshift
	 * by 8 after rounding up. We also have to account for PCIe link speed
	 * difference as ITR scales based on this.
	 */
	if (avg_wire_size <= 360) {
		/* Start at 250K ints/sec and gradually drop to 77K ints/sec */
		avg_wire_size *= 8;
		avg_wire_size += 376;
	} else if (avg_wire_size <= 1152) {
		/* 77K ints/sec to 45K ints/sec */
		avg_wire_size *= 3;
		avg_wire_size += 2176;
	} else if (avg_wire_size <= 1920) {
		/* 45K ints/sec to 38K ints/sec */
		avg_wire_size += 4480;
	} else {
		/* plateau at a limit of 38K ints/sec */
		avg_wire_size = 6656;
	}

	/* Perform final bitshift for division after rounding up to ensure
	 * that the calculation will never get below a 1. The bit shift
	 * accounts for changes in the ITR due to PCIe link speed.
	 */
	itr_round = READ_ONCE(ring_container->itr_scale) + 8;
	avg_wire_size += BIT(itr_round) - 1;
	avg_wire_size >>= itr_round;

	/* write back value and retain adaptive flag */
	ring_container->itr = avg_wire_size | FM10K_ITR_ADAPTIVE;

clear_counts:
	ring_container->total_bytes = 0;
	ring_container->total_packets = 0;
}