int mingw_fgetc(FILE *stream)
{
	int ch;
	if (!isatty(_fileno(stream)))
		return fgetc(stream);

	SetConsoleCtrlHandler(ctrl_ignore, TRUE);
	while (1) {
		ch = fgetc(stream);
		if (ch != EOF || GetLastError() != ERROR_OPERATION_ABORTED)
			break;

		/* Ctrl+C was pressed, simulate SIGINT and retry */
		mingw_raise(SIGINT);
	}
	SetConsoleCtrlHandler(ctrl_ignore, FALSE);
	return ch;
}