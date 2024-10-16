        std::vector<char*> get_list(const std::string& name, bool use_brackets = true) const
        {
            std::vector<char*> ret;
            std::string plus = name + (use_brackets ? "[]" : "");
            char* element = nullptr;

            int count = 0;
            while (1)
            {
                element = qs_k2v(plus.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++);
                if (!element)
                    break;
                ret.push_back(element);
            }
            return ret;
        }