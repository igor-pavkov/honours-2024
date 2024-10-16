static void fm10k_qv_enable(struct fm10k_q_vector *q_vector)
{
	/* Enable auto-mask and clear the current mask */
	u32 itr = FM10K_ITR_ENABLE;

	/* Update Tx ITR */
	fm10k_update_itr(&q_vector->tx);

	/* Update Rx ITR */
	fm10k_update_itr(&q_vector->rx);

	/* Store Tx itr in timer slot 0 */
	itr |= (q_vector->tx.itr & FM10K_ITR_MAX);

	/* Shift Rx itr to timer slot 1 */
	itr |= (q_vector->rx.itr & FM10K_ITR_MAX) << FM10K_ITR_INTERVAL1_SHIFT;

	/* Write the final value to the ITR register */
	writel(itr, q_vector->itr);
}