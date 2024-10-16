static u8 *uvesafb_vbe_state_save(struct uvesafb_par *par)
{
	struct uvesafb_ktask *task;
	u8 *state;
	int err;

	if (!par->vbe_state_size)
		return NULL;

	state = kmalloc(par->vbe_state_size, GFP_KERNEL);
	if (!state)
		return ERR_PTR(-ENOMEM);

	task = uvesafb_prep();
	if (!task) {
		kfree(state);
		return NULL;
	}

	task->t.regs.eax = 0x4f04;
	task->t.regs.ecx = 0x000f;
	task->t.regs.edx = 0x0001;
	task->t.flags = TF_BUF_RET | TF_BUF_ESBX;
	task->t.buf_len = par->vbe_state_size;
	task->buf = state;
	err = uvesafb_exec(task);

	if (err || (task->t.regs.eax & 0xffff) != 0x004f) {
		pr_warn("VBE get state call failed (eax=0x%x, err=%d)\n",
			task->t.regs.eax, err);
		kfree(state);
		state = NULL;
	}

	uvesafb_free(task);
	return state;
}