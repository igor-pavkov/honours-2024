    std::string::size_type findi(const std::string& str, const std::string& substr)
    {
        return upper(str).find(upper(substr));
    }