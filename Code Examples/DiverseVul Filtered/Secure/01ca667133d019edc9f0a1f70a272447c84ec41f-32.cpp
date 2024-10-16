u64 fm10k_get_tx_pending(struct fm10k_ring *ring, bool in_sw)
{
	struct fm10k_intfc *interface = ring->q_vector->interface;
	struct fm10k_hw *hw = &interface->hw;
	u32 head, tail;

	if (likely(in_sw)) {
		head = ring->next_to_clean;
		tail = ring->next_to_use;
	} else {
		head = fm10k_read_reg(hw, FM10K_TDH(ring->reg_idx));
		tail = fm10k_read_reg(hw, FM10K_TDT(ring->reg_idx));
	}

	return ((head <= tail) ? tail : tail + ring->count) - head;
}