	void On005Numeric(std::map<std::string, std::string>& tokens) CXX11_OVERRIDE
	{
		tokens["ESILENCE"] = "CcdiNnPpTtx";
		tokens["SILENCE"] = ConvToStr(cmd.maxsilence);
	}