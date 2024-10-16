static void clamp_pollexp_and_set_MAXSTRAT(void)
{
	if (G.poll_exp < MINPOLL)
		G.poll_exp = MINPOLL;
	if (G.poll_exp > BIGPOLL)
		G.poll_exp = BIGPOLL;
	G.polladj_count = 0;
	G.stratum = MAXSTRAT;
}