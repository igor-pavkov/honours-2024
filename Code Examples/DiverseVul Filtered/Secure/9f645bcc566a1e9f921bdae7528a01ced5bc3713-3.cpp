static void uvesafb_vbe_state_restore(struct uvesafb_par *par, u8 *state_buf)
{
	struct uvesafb_ktask *task;
	int err;

	if (!state_buf)
		return;

	task = uvesafb_prep();
	if (!task)
		return;

	task->t.regs.eax = 0x4f04;
	task->t.regs.ecx = 0x000f;
	task->t.regs.edx = 0x0002;
	task->t.buf_len = par->vbe_state_size;
	task->t.flags = TF_BUF_ESBX;
	task->buf = state_buf;

	err = uvesafb_exec(task);
	if (err || (task->t.regs.eax & 0xffff) != 0x004f)
		pr_warn("VBE state restore call failed (eax=0x%x, err=%d)\n",
			task->t.regs.eax, err);

	uvesafb_free(task);
}