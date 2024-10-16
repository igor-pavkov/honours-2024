        query_string& operator=(const query_string& qs)
        {
            url_ = qs.url_;
            key_value_pairs_.clear();
            for (auto p : qs.key_value_pairs_)
            {
                key_value_pairs_.push_back((char*)(p - qs.url_.c_str() + url_.c_str()));
            }
            return *this;
        }