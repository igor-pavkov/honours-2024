void CommentParser::collectParameters(const std::string& fulltext, SourceFile *root_file)
{
	static auto EmptyStringLiteral(std::make_shared<Literal>(""));

	// Get all groups of parameters
	GroupList groupList = collectGroups(fulltext);
	int parseTill=getLineToStop(fulltext);
	// Extract parameters for all literal assignments
	for (auto &assignment : root_file->scope.assignments) {
		if (!assignment->getExpr()->isLiteral()) continue; // Only consider literals

		// get location of assignment node
		int firstLine = assignment->location().firstLine();
		if(firstLine>=parseTill || (
			assignment->location().fileName() != "" &&
			assignment->location().fileName() != root_file->getFilename() &&
			assignment->location().fileName() != root_file->getFullpath()
			)) {
			continue;
		}
		// making list to add annotations
		AnnotationList *annotationList = new AnnotationList();

		// Extracting the parameter comment
		std::string comment = getComment(fulltext, firstLine);
		// getting the node for parameter annotation
		shared_ptr<Expression> params = CommentParser::parser(comment.c_str());
		if (!params) {
			params = EmptyStringLiteral;
		}

		// adding parameter to the list
		annotationList->push_back(Annotation("Parameter", params));

		//extracting the description
		std::string descr = getDescription(fulltext, firstLine - 1);
		if (descr != "") {
			//creating node for description
			shared_ptr<Expression> expr(new Literal(descr));
			annotationList->push_back(Annotation("Description", expr));
		}

		// Look for the group to which the given assignment belong
		for (const auto &groupInfo :boost::adaptors::reverse(groupList)){
			if (groupInfo.lineNo < firstLine) {
				//creating node for description
				shared_ptr<Expression> expr(new Literal(groupInfo.commentString));
				annotationList->push_back(Annotation("Group", expr));
				break;
			}
		}
		assignment->addAnnotations(annotationList);
	}
}