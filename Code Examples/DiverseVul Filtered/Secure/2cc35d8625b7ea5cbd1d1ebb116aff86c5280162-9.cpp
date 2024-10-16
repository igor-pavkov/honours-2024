	void submit(SQLQuery* call, const std::string& q, const ParamL& p)
	{
		std::string res;
		unsigned int param = 0;
		for(std::string::size_type i = 0; i < q.length(); i++)
		{
			if (q[i] != '?')
				res.push_back(q[i]);
			else
			{
				if (param < p.size())
				{
					std::string parm = p[param++];
					// In the worst case, each character may need to be encoded as using two bytes,
					// and one byte is the terminating null
					std::vector<char> buffer(parm.length() * 2 + 1);

					// The return value of mysql_escape_string() is the length of the encoded string,
					// not including the terminating null
					unsigned long escapedsize = mysql_escape_string(&buffer[0], parm.c_str(), parm.length());
//					mysql_real_escape_string(connection, queryend, paramscopy[paramnum].c_str(), paramscopy[paramnum].length());
					res.append(&buffer[0], escapedsize);
				}
			}
		}
		submit(call, res);
	}