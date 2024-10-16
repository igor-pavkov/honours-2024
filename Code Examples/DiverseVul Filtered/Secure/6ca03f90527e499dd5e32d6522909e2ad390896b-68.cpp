static void k_dead(struct vc_data *vc, unsigned char value, char up_flag)
{
	static const unsigned char ret_diacr[NR_DEAD] = {
		'`',	/* dead_grave */
		'\'',	/* dead_acute */
		'^',	/* dead_circumflex */
		'~',	/* dead_tilda */
		'"',	/* dead_diaeresis */
		',',	/* dead_cedilla */
		'_',	/* dead_macron */
		'U',	/* dead_breve */
		'.',	/* dead_abovedot */
		'*',	/* dead_abovering */
		'=',	/* dead_doubleacute */
		'c',	/* dead_caron */
		'k',	/* dead_ogonek */
		'i',	/* dead_iota */
		'#',	/* dead_voiced_sound */
		'o',	/* dead_semivoiced_sound */
		'!',	/* dead_belowdot */
		'?',	/* dead_hook */
		'+',	/* dead_horn */
		'-',	/* dead_stroke */
		')',	/* dead_abovecomma */
		'(',	/* dead_abovereversedcomma */
		':',	/* dead_doublegrave */
		'n',	/* dead_invertedbreve */
		';',	/* dead_belowcomma */
		'$',	/* dead_currency */
		'@',	/* dead_greek */
	};

	k_deadunicode(vc, ret_diacr[value], up_flag);
}