        std::unordered_map<std::string, std::string> pop_dict(const std::string& name)
        {
            std::unordered_map<std::string, std::string> ret = get_dict(name);
            if (!ret.empty())
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if (str_item.substr(0, name.size() + 1) == name + '[')
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i--);
                    }
                }
            }
            return ret;
        }