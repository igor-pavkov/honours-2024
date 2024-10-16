        friend std::ostream& operator<<(std::ostream& os, const query_string& qs)
        {
            os << "[ ";
            for (size_t i = 0; i < qs.key_value_pairs_.size(); ++i)
            {
                if (i)
                    os << ", ";
                os << qs.key_value_pairs_[i];
            }
            os << " ]";
            return os;
        }