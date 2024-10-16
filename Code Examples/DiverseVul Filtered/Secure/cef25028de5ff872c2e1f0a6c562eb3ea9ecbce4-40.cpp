	bool testToStringCharsRequiredHelper(const wchar_t * text) {
		// Parse
		UriParserStateW state;
		UriUriW uri;
		state.uri = &uri;
		int res = uriParseUriW(&state, text);
		if (res != 0) {
			uriFreeUriMembersW(&uri);
			return false;
		}

		// Required space?
		int charsRequired;
		if (uriToStringCharsRequiredW(&uri, &charsRequired) != 0) {
			uriFreeUriMembersW(&uri);
			return false;
		}

		EXPECT_EQ(charsRequired, wcslen(text));

		// Minimum
		wchar_t * buffer = new wchar_t[charsRequired + 1];
		if (uriToStringW(buffer, &uri, charsRequired + 1, NULL) != 0) {
			uriFreeUriMembersW(&uri);
			delete [] buffer;
			return false;
		}

		// One less than minimum
		if (uriToStringW(buffer, &uri, charsRequired, NULL) == 0) {
			uriFreeUriMembersW(&uri);
			delete [] buffer;
			return false;
		}

		uriFreeUriMembersW(&uri);
		delete [] buffer;
		return true;
	}