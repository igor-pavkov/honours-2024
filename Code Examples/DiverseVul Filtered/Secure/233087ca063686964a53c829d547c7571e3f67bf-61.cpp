static void reset_interrupt(void)
{
	debugt(__func__, "");
	result(current_fdc);		/* get the status ready for set_fdc */
	if (fdc_state[current_fdc].reset) {
		pr_info("reset set in interrupt, calling %ps\n", cont->error);
		cont->error();	/* a reset just after a reset. BAD! */
	}
	cont->redo();
}