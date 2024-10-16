static GroupInfo createGroup(std::string comment,int lineNo)
{
	//store info related to group
	GroupInfo groupInfo;
	std::string finalGroupName;

	boost::regex regex("\\[(.*?)\\]");
	boost::match_results<std::string::const_iterator>  match;
	while(boost::regex_search(comment, match, regex)) {
		std::string groupName = match[1].str();
		if (finalGroupName.empty()) {
			finalGroupName = groupName;
		} else {
			finalGroupName = finalGroupName + "-" + groupName;
		}
		groupName.clear();
		comment = match.suffix();
	}

	groupInfo.commentString = finalGroupName;
	groupInfo.lineNo = lineNo;
	return groupInfo;
}