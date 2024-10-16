vector<string> IRC_Analyzer::SplitWords(const string input, const char split)
	{
	vector<string> words;

	if ( input.size() < 1 )
		return words;

	unsigned int start = 0;
	unsigned int split_pos = 0;

	// Ignore split-characters at the line beginning.
	while ( input[start] == split )
		{
		++start;
		++split_pos;
		}

	string word = "";
	while ( (split_pos = input.find(split, start)) < input.size() )
		{
		word = input.substr(start, split_pos - start);
		if ( word.size() > 0 && word[0] != split )
			words.push_back(word);

		start = split_pos + 1;
		}

	// Add line end if needed.
	if ( start < input.size() )
		{
		word = input.substr(start, input.size() - start);
		words.push_back(word);
		}

	return words;
	}