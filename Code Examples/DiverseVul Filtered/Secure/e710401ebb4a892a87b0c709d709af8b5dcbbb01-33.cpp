R_API int r_egg_padding(REgg *egg, const char *pad) {
	int number;
	ut8 *buf, padding_byte;
	char *p, *o = strdup (pad);

	for (p = o; *p;) { // parse pad string
		const char f = *p++;
		number = strtol (p, NULL, 10);

		if (number < 1) {
			eprintf ("Invalid padding length at %d\n", number);
			free (o);
			return false;
		}
		p = eon(p);

		switch (f) {
		case 's': case 'S': padding_byte = 0; break;
		case 'n': case 'N': padding_byte = 0x90; break;
		case 'a':
		case 'A': padding_byte = 'A'; break;
		case 't':
		case 'T': padding_byte = 0xcc; break;
		default:
			eprintf ("Invalid padding format (%c)\n", *p);
			eprintf ("Valid ones are:\n");
			eprintf ("	s S : NULL byte");
			eprintf ("	n N : nop");
			eprintf ("	a A : 0x41");
			eprintf ("	t T : trap (0xcc)");
			free (o);
			return false;
		}

		buf = malloc (number);
		if (!buf) {
			free (o);
			return false;
		}

		memset (buf, padding_byte, number);
		if (f >= 'a' && f <= 'z') {
			r_egg_prepend_bytes(egg, buf, number);
		} else {
			r_egg_append_bytes(egg, buf, number);
		}
		free (buf);
	}
	free (o);
	return true;
}