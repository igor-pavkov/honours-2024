static void fm10k_free_q_vector(struct fm10k_intfc *interface, int v_idx)
{
	struct fm10k_q_vector *q_vector = interface->q_vector[v_idx];
	struct fm10k_ring *ring;

	fm10k_dbg_q_vector_exit(q_vector);

	fm10k_for_each_ring(ring, q_vector->tx)
		interface->tx_ring[ring->queue_index] = NULL;

	fm10k_for_each_ring(ring, q_vector->rx)
		interface->rx_ring[ring->queue_index] = NULL;

	interface->q_vector[v_idx] = NULL;
	netif_napi_del(&q_vector->napi);
	kfree_rcu(q_vector, rcu);
}