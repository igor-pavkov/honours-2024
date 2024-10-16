static int getLineToStop( const std::string &fulltext){
	int lineNo=1;
	bool inString=false;
	for (unsigned int i=0; i<fulltext.length(); ++i) {
	// increase line number
		if (fulltext[i] == '\n') {
			lineNo++;
			continue;
		}

		// skip escaped quotes inside strings
		if (inString && fulltext.compare(i, 2, "\\\"") == 0) {
			i++;
			continue;
		}

		//start or end of string negate the checkpoint
		if (fulltext[i] == '"') {
			inString = !inString;
			continue;
		}

		if (!inString && fulltext.compare(i, 2, "//") == 0) {
			i++;
			while (fulltext[i] != '\n' && i<fulltext.length()) i++;
			lineNo++;
			continue;
		}

		//start of multi line comment if check is true
		if (!inString && fulltext.compare(i, 2, "/*") == 0) {
			i ++;
			if(i<fulltext.length()) {
				i++;
			}
			else {
				continue;
			}
			// till */ every character is comment
			while (fulltext.compare(i, 2, "*/") != 0 && i<fulltext.length()) {
				if(fulltext[i]=='\n'){
					lineNo++;
				}
				i++;
			}
		}

		if (fulltext[i]== '{') {
			return lineNo;
		}
	}
	return lineNo;
}