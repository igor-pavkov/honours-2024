static void use_in_memory_ccache(void) {
	setenv(KRB5_ENV_CCNAME, "MEMORY:cliconnect", 1);
}