	static std::string BitsToFlags(uint32_t flags)
	{
		std::string out;
		if (flags & SF_CTCP_USER)
			out.push_back('C');
		if (flags & SF_CTCP_CHANNEL)
			out.push_back('c');
		if (flags & SF_INVITE)
			out.push_back('i');
		if (flags & SF_NOTICE_USER)
			out.push_back('N');
		if (flags & SF_NOTICE_CHANNEL)
			out.push_back('n');
		if (flags & SF_PRIVMSG_USER)
			out.push_back('P');
		if (flags & SF_PRIVMSG_CHANNEL)
			out.push_back('p');
		if (flags & SF_TAGMSG_CHANNEL)
			out.push_back('T');
		if (flags & SF_TAGMSG_USER)
			out.push_back('t');
		if (flags & SF_EXEMPT)
			out.push_back('x');
		return out;
	}