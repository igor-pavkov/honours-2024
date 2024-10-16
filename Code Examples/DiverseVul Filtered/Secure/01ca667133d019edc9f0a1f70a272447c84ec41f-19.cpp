bool fm10k_check_tx_hang(struct fm10k_ring *tx_ring)
{
	u32 tx_done = fm10k_get_tx_completed(tx_ring);
	u32 tx_done_old = tx_ring->tx_stats.tx_done_old;
	u32 tx_pending = fm10k_get_tx_pending(tx_ring, true);

	clear_check_for_tx_hang(tx_ring);

	/* Check for a hung queue, but be thorough. This verifies
	 * that a transmit has been completed since the previous
	 * check AND there is at least one packet pending. By
	 * requiring this to fail twice we avoid races with
	 * clearing the ARMED bit and conditions where we
	 * run the check_tx_hang logic with a transmit completion
	 * pending but without time to complete it yet.
	 */
	if (!tx_pending || (tx_done_old != tx_done)) {
		/* update completed stats and continue */
		tx_ring->tx_stats.tx_done_old = tx_done;
		/* reset the countdown */
		clear_bit(__FM10K_HANG_CHECK_ARMED, tx_ring->state);

		return false;
	}

	/* make sure it is true for two checks in a row */
	return test_and_set_bit(__FM10K_HANG_CHECK_ARMED, tx_ring->state);
}