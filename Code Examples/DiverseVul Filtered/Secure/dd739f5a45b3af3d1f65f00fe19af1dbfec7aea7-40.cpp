static bool exec_command_on_flag(RFlagItem *flg, void *u) {
	struct exec_command_t *user = (struct exec_command_t *)u;
	r_core_block_size (user->core, flg->size);
	r_core_seek (user->core, flg->offset, 1);
	r_core_cmd0 (user->core, user->cmd);
	return true;
}