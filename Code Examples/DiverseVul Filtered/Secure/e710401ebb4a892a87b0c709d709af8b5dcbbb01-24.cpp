R_API void r_egg_free(REgg *egg) {
	if (egg) {
		r_buf_free (egg->src);
		r_buf_free (egg->buf);
		r_buf_free (egg->bin);
		r_list_free (egg->list);
		r_asm_free (egg->rasm);
		r_syscall_free (egg->syscall);
		sdb_free (egg->db);
		r_list_free (egg->plugins);
		r_list_free (egg->patches);
		r_egg_lang_free (egg);
		free (egg);
	}
}