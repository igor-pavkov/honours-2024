URI_CHAR URI_FUNC(HexToLetter)(unsigned int value) {
	/* Uppercase recommended in section 2.1. of RFC 3986 *
	 * http://tools.ietf.org/html/rfc3986#section-2.1    */
	return URI_FUNC(HexToLetterEx)(value, URI_TRUE);
}