TEST(UriSuite, TestQueryCompositionMathWriteGoogleAutofuzz113244572) {
		UriQueryListA second = { /*.key =*/ "\x11", /*.value =*/ NULL, /*.next =*/ NULL };
		UriQueryListA first = { /*.key =*/ "\x01", /*.value =*/ "\x02", /*.next =*/ &second };

		const UriBool spaceToPlus = URI_TRUE;
		const UriBool normalizeBreaks = URI_FALSE;  /* for factor 3 but 6 */

		const int charsRequired = (3 + 1 + 3) + 1 + (3);

		{
			// Minimum space to hold everything fine
			const char * const expected = "%01=%02" "&" "%11";
			char dest[charsRequired + 1];
			int charsWritten;
			ASSERT_TRUE(uriComposeQueryExA(dest, &first, sizeof(dest),
					&charsWritten, spaceToPlus, normalizeBreaks)
				== URI_SUCCESS);
			ASSERT_TRUE(! strcmp(dest, expected));
			ASSERT_TRUE(charsWritten == strlen(expected) + 1);
		}

		{
			// Previous math failed to take ampersand into account
			char dest[charsRequired + 1 - 1];
			int charsWritten;
			ASSERT_TRUE(uriComposeQueryExA(dest, &first, sizeof(dest),
					&charsWritten, spaceToPlus, normalizeBreaks)
				== URI_ERROR_OUTPUT_TOO_LARGE);
		}
}