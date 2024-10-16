static void abort_unzip(STATE_PARAM_ONLY)
{
	huft_free_all(PASS_STATE_ONLY);
	longjmp(error_jmp, 1);
}