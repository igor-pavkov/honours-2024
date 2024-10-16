static char *StrTok(
	/*! String containing the token. */
	char **Src,
	/*! Set of delimiter characters. */
	const char *Del)
{
	char *TmpPtr;
	char *RetPtr;

	if (*Src != NULL) {
		RetPtr = *Src;
		TmpPtr = strstr(*Src, Del);
		if (TmpPtr != NULL) {
			*TmpPtr = '\0';
			*Src = TmpPtr + strlen(Del);
		} else
			*Src = NULL;

		return RetPtr;
	}

	return NULL;
}