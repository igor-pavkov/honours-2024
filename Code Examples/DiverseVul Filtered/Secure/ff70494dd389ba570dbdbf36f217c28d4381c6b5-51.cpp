void textstyle(int attr)
{	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "\033[%im", attr);
	fprintf(stderr, "%s", command);
	fflush(stderr);
}