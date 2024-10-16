inline void IRC_Analyzer::SkipLeadingWhitespace(string& str)
	{
	const auto first_char = str.find_first_not_of(" ");
	if ( first_char == string::npos )
		str = "";
	else
		str = str.substr(first_char);
	}