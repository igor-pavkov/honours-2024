        std::vector<char*> pop_list(const std::string& name, bool use_brackets = true)
        {
            std::vector<char*> ret = get_list(name, use_brackets);
            if (!ret.empty())
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if ((use_brackets ? (str_item.substr(0, name.size() + 3) == name + "[]=") : (str_item.substr(0, name.size() + 1) == name + '=')))
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i--);
                    }
                }
            }
            return ret;
        }