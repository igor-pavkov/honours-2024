static std::string getDescription(const std::string &fulltext, int line)
{
	if (line < 1) return "";

	unsigned int start = 0;
	for (; start<fulltext.length() ; ++start) {
		if (line <= 1) break;
		if (fulltext[start] == '\n') line--;
	}

	// not a valid description
	if (fulltext.compare(start, 2, "//") != 0) return "";

	// Jump over the two forward slashes
	start = start+2;

	//Jump over all the spaces
	while (fulltext[start] == ' ' || fulltext[start] == '\t') start++;
	std::string retString = "";

	// go till the end of the line
	while (fulltext[start] != '\n') {
		// replace // with space
		if (fulltext.compare(start, 2, "//") == 0) {
			retString += " ";
			start++;
		} else {
			retString += fulltext[start];
		}
		start++;
	}
	return retString;
}