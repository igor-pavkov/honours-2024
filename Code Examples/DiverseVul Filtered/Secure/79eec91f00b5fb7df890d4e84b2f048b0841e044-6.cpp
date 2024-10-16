        query_string& operator=(query_string&& qs)
        {
            key_value_pairs_ = std::move(qs.key_value_pairs_);
            char* old_data = (char*)qs.url_.c_str();
            url_ = std::move(qs.url_);
            for (auto& p : key_value_pairs_)
            {
                p += (char*)url_.c_str() - old_data;
            }
            return *this;
        }