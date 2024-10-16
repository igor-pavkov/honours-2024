void gf_prompt_set_echo_off(Bool echo_off)
{
	DWORD flags;
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	BOOL ret = GetConsoleMode(hStdin, &flags);
	if (!ret) {
		DWORD err = GetLastError();
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONSOLE, ("[Console] GetConsoleMode() return with the following error code: %d\n", err));
	}
	if (echo_off) flags &= ~ENABLE_ECHO_INPUT;
	else flags |= ENABLE_ECHO_INPUT;
	SetConsoleMode(hStdin, flags);
}