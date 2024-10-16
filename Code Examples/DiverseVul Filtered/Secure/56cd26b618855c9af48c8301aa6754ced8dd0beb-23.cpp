static void send_pulse_homebrew(unsigned int length, ktime_t edge)
{
	if (softcarrier)
		send_pulse_homebrew_softcarrier(length, edge);
	else
		on();
}