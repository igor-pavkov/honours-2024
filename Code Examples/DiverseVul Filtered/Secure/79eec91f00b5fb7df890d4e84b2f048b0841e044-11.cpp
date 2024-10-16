        std::vector<std::string> keys() const
        {
            std::vector<std::string> ret;
            for (auto element : key_value_pairs_)
            {
                std::string str_element(element);
                ret.emplace_back(str_element.substr(0, str_element.find('=')));
            }
            return ret;
        }