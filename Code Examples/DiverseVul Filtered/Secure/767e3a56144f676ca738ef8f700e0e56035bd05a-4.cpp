    bool SGeometry::has_next_geometry()
    {
        if(this->cur_geometry_ind < node_counts.size())
        {
            return true;
        }
        else return false;
    }