        char* pop(const std::string& name)
        {
            char* ret = get(name);
            if (ret != nullptr)
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if (str_item.substr(0, name.size() + 1) == name + '=')
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i);
                        break;
                    }
                }
            }
            return ret;
        }