static NORETURN void die_startup(void)
{
	fputs("fatal: not enough memory for initialization", stderr);
	exit(128);
}