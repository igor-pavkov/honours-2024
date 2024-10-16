static GroupList collectGroups(const std::string &fulltext)
{
	GroupList groupList; // container of all group names
	int lineNo = 1; // tracks line number
	bool inString = false; // check if its string or (line-) comment

	// iterate through whole scad file
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
			while (fulltext[i] != '\n' && i<fulltext.length() ) i++;
			lineNo++;
			continue;
		}

		//start of multi line comment if check is true
		if (!inString && fulltext.compare(i, 2, "/*") == 0) {
			//store comment
			std::string comment;
			i++;
			if(i<fulltext.length()) {
				i++;
			}
			else {
				continue;
			}
			bool isGroup=true;
			// till */ every character is comment
			while (fulltext.compare(i, 2, "*/") != 0 && i<fulltext.length()) {
				if(fulltext[i]=='\n'){
					lineNo++;
					isGroup=false;
				}
				comment += fulltext[i];
				i++;
			}

			if(isGroup)
				groupList.push_back(createGroup(comment,lineNo));
		}
	}
	return groupList;
}