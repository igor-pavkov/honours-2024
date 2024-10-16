	void submit(SQLQuery* call, const std::string& q, const ParamM& p)
	{
		std::string res;
		for(std::string::size_type i = 0; i < q.length(); i++)
		{
			if (q[i] != '$')
				res.push_back(q[i]);
			else
			{
				std::string field;
				i++;
				while (i < q.length() && isalnum(q[i]))
					field.push_back(q[i++]);
				i--;

				ParamM::const_iterator it = p.find(field);
				if (it != p.end())
				{
					std::string parm = it->second;
					// NOTE: See above
					std::vector<char> buffer(parm.length() * 2 + 1);
					unsigned long escapedsize = mysql_escape_string(&buffer[0], parm.c_str(), parm.length());
					res.append(&buffer[0], escapedsize);
				}
			}
		}
		submit(call, res);
	}