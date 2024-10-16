void signal_setup_done(int failed, struct ksignal *ksig, int stepping)
{
	if (failed)
		force_sigsegv(ksig->sig, current);
	else
		signal_delivered(ksig->sig, &ksig->info, &ksig->ka,
			signal_pt_regs(), stepping);
}