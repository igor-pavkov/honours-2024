static int cmd_last(void *data, const char *input) {
	switch (*input) {
	case 0:
		r_cons_last ();
		break;
	default:
		eprintf ("Usage: _  print last output\n");
	}
	return 0;
}