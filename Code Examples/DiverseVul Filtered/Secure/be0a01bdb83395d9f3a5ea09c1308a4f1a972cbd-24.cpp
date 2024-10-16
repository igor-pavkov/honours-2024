static int GetNextRange(
	/*! string containing the token / range. */
	char **SrcRangeStr,
	/*! gets the first byte of the token. */
	off_t *FirstByte,
	/*! gets the last byte of the token. */
	off_t *LastByte)
{
	char *Ptr;
	char *Tok;
	int i;
	int64_t F = -1;
	int64_t L = -1;
	int Is_Suffix_byte_Range = 1;

	if (*SrcRangeStr == NULL)
		return -1;
	Tok = StrTok(SrcRangeStr, ",");
	if ((Ptr = strstr(Tok, "-")) == NULL)
		return -1;
	*Ptr = ' ';
	sscanf(Tok, "%" SCNd64 "%" SCNd64, &F, &L);
	if (F == -1 || L == -1) {
		*Ptr = '-';
		for (i = 0; i < (int)strlen(Tok); i++) {
			if (Tok[i] == '-') {
				break;
			} else if (isdigit(Tok[i])) {
				Is_Suffix_byte_Range = 0;
				break;
			}
		}
		if (Is_Suffix_byte_Range) {
			*FirstByte = (off_t) L;
			*LastByte = (off_t) F;
			return 1;
		}
	}
	*FirstByte = (off_t) F;
	*LastByte = (off_t) L;

	return 1;
}