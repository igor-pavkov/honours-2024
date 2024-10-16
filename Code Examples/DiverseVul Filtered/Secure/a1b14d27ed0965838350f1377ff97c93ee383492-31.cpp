static void mark_reg_unknown_value(struct reg_state *regs, u32 regno)
{
	BUG_ON(regno >= MAX_BPF_REG);
	regs[regno].type = UNKNOWN_VALUE;
	regs[regno].imm = 0;
	regs[regno].map_ptr = NULL;
}