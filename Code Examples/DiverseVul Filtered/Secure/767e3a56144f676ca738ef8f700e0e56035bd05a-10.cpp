    void SGeometry::next_geometry()
    {
        // to do: maybe implement except. and such on error conds.

        this->cur_geometry_ind++;
        this->cur_part_ind = 0;
        this->current_vert_ind = 0;    
    }