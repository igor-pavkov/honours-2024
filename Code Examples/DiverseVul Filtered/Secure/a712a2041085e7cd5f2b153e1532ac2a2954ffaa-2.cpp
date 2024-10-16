static int jpc_dec_process_unk(jpc_dec_t *dec, jpc_ms_t *ms)
{
	/* Eliminate compiler warnings about unused variables. */
	dec = 0;

	jas_eprintf("warning: ignoring unknown marker segment (0x%x)\n",
	  ms->id);
	jpc_ms_dump(ms, stderr);
	return 0;
}