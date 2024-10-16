void textcolor_fg(int fg)
{	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "\033[%dm", fg + 30);
	fprintf(stderr, "%s", command);
	fflush(stderr);
}