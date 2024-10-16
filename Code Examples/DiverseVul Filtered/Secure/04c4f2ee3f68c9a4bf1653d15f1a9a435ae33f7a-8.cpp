static inline void pt_load_msr(struct pt_ctx *ctx, u32 addr_range)
{
	u32 i;

	wrmsrl(MSR_IA32_RTIT_STATUS, ctx->status);
	wrmsrl(MSR_IA32_RTIT_OUTPUT_BASE, ctx->output_base);
	wrmsrl(MSR_IA32_RTIT_OUTPUT_MASK, ctx->output_mask);
	wrmsrl(MSR_IA32_RTIT_CR3_MATCH, ctx->cr3_match);
	for (i = 0; i < addr_range; i++) {
		wrmsrl(MSR_IA32_RTIT_ADDR0_A + i * 2, ctx->addr_a[i]);
		wrmsrl(MSR_IA32_RTIT_ADDR0_B + i * 2, ctx->addr_b[i]);
	}
}