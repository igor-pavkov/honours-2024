        std::unordered_map<std::string, std::string> get_dict(const std::string& name) const
        {
            std::unordered_map<std::string, std::string> ret;

            int count = 0;
            while (1)
            {
                if (auto element = qs_dict_name2kv(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++))
                    ret.insert(*element);
                else
                    break;
            }
            return ret;
        }