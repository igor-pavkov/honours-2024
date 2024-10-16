	bool operator <(const SilenceEntry& other) const
	{
		if (flags & SF_EXEMPT && other.flags & ~SF_EXEMPT)
			return true;
		if (other.flags & SF_EXEMPT && flags & ~SF_EXEMPT)
			return false;
		if (flags < other.flags)
			return true;
		if (other.flags < flags)
			return false;
		return mask < other.mask;
	}