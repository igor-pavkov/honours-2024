static int __init setup_psi(char *str)
{
	return kstrtobool(str, &psi_enable) == 0;
}