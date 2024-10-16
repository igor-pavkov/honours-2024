static Exiv2::Dictionary stringToDict(const std::string& s)
{
    Exiv2::Dictionary result;
    std::string   token;

    size_t i = 0;
    while (i < s.length() )
    {
        if ( s[i] != ',' ) {
            if ( s[i] != ' ' )
                token += s[i];
        } else {
            result[token]=token;
            token.clear();
        }
        i++;
    }
    result[token]=token;
    return result;
}