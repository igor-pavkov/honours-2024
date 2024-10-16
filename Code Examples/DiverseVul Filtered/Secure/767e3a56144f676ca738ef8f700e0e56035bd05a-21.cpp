    bool SGeometry::has_next_pt()
    {
        if(this->current_vert_ind < node_counts[cur_geometry_ind])
        {
            return true;
        }
    
        else return false;
    }