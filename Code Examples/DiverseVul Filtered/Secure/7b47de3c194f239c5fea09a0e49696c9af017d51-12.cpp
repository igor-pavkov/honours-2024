	static bool FlagsToBits(const std::string& flags, uint32_t& out)
	{
		out = SF_NONE;
		for (std::string::const_iterator flag = flags.begin(); flag != flags.end(); ++flag)
		{
			switch (*flag)
			{
				case 'C':
					out |= SF_CTCP_USER;
					break;
				case 'c':
					out |= SF_CTCP_CHANNEL;
					break;
				case 'd':
					out |= SF_DEFAULT;
					break;
				case 'i':
					out |= SF_INVITE;
					break;
				case 'N':
					out |= SF_NOTICE_USER;
					break;
				case 'n':
					out |= SF_NOTICE_CHANNEL;
					break;
				case 'P':
					out |= SF_PRIVMSG_USER;
					break;
				case 'p':
					out |= SF_PRIVMSG_CHANNEL;
					break;
				case 'T':
					out |= SF_TAGMSG_USER;
					break;
				case 't':
					out |= SF_TAGMSG_CHANNEL;
					break;
				case 'x':
					out |= SF_EXEMPT;
					break;
				default:
					out = SF_NONE;
					return false;
			}
		}
		return true;
	}