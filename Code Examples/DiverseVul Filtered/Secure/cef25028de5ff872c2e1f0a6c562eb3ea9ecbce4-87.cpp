TEST(UriSuite, TestQueryCompositionMathCalc) {
		UriQueryListA second = { /*.key =*/ "k2", /*.value =*/ "v2", /*.next =*/ NULL };
		UriQueryListA first = { /*.key =*/ "k1", /*.value =*/ "v1", /*.next =*/ &second };

		int charsRequired;
		ASSERT_TRUE(uriComposeQueryCharsRequiredA(&first, &charsRequired)
				== URI_SUCCESS);

		const int FACTOR = 6;  /* due to escaping with normalizeBreaks */
		ASSERT_TRUE((unsigned)charsRequired ==
			FACTOR * strlen(first.key) + 1 + FACTOR * strlen(first.value)
			+ 1
			+ FACTOR * strlen(second.key) + 1 + FACTOR * strlen(second.value)
		);
}