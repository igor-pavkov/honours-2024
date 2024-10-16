        char* get(const std::string& name) const
        {
            char* ret = qs_k2v(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size());
            return ret;
        }