static int fm10k_alloc_q_vector(struct fm10k_intfc *interface,
				unsigned int v_count, unsigned int v_idx,
				unsigned int txr_count, unsigned int txr_idx,
				unsigned int rxr_count, unsigned int rxr_idx)
{
	struct fm10k_q_vector *q_vector;
	struct fm10k_ring *ring;
	int ring_count;

	ring_count = txr_count + rxr_count;

	/* allocate q_vector and rings */
	q_vector = kzalloc(struct_size(q_vector, ring, ring_count), GFP_KERNEL);
	if (!q_vector)
		return -ENOMEM;

	/* initialize NAPI */
	netif_napi_add(interface->netdev, &q_vector->napi,
		       fm10k_poll, NAPI_POLL_WEIGHT);

	/* tie q_vector and interface together */
	interface->q_vector[v_idx] = q_vector;
	q_vector->interface = interface;
	q_vector->v_idx = v_idx;

	/* initialize pointer to rings */
	ring = q_vector->ring;

	/* save Tx ring container info */
	q_vector->tx.ring = ring;
	q_vector->tx.work_limit = FM10K_DEFAULT_TX_WORK;
	q_vector->tx.itr = interface->tx_itr;
	q_vector->tx.itr_scale = interface->hw.mac.itr_scale;
	q_vector->tx.count = txr_count;

	while (txr_count) {
		/* assign generic ring traits */
		ring->dev = &interface->pdev->dev;
		ring->netdev = interface->netdev;

		/* configure backlink on ring */
		ring->q_vector = q_vector;

		/* apply Tx specific ring traits */
		ring->count = interface->tx_ring_count;
		ring->queue_index = txr_idx;

		/* assign ring to interface */
		interface->tx_ring[txr_idx] = ring;

		/* update count and index */
		txr_count--;
		txr_idx += v_count;

		/* push pointer to next ring */
		ring++;
	}

	/* save Rx ring container info */
	q_vector->rx.ring = ring;
	q_vector->rx.itr = interface->rx_itr;
	q_vector->rx.itr_scale = interface->hw.mac.itr_scale;
	q_vector->rx.count = rxr_count;

	while (rxr_count) {
		/* assign generic ring traits */
		ring->dev = &interface->pdev->dev;
		ring->netdev = interface->netdev;
		rcu_assign_pointer(ring->l2_accel, interface->l2_accel);

		/* configure backlink on ring */
		ring->q_vector = q_vector;

		/* apply Rx specific ring traits */
		ring->count = interface->rx_ring_count;
		ring->queue_index = rxr_idx;

		/* assign ring to interface */
		interface->rx_ring[rxr_idx] = ring;

		/* update count and index */
		rxr_count--;
		rxr_idx += v_count;

		/* push pointer to next ring */
		ring++;
	}

	fm10k_dbg_q_vector_init(q_vector);

	return 0;
}