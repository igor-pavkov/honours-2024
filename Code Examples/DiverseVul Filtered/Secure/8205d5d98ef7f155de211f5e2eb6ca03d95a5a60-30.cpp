static void print_bits(unsigned long val, struct regbit *bits, const char *sep)
{
	const char *s = "";

	for (; bits->bit; ++bits)
		if (val & bits->bit) {
			pr_cont("%s%s", s, bits->name);
			s = sep;
		}
}