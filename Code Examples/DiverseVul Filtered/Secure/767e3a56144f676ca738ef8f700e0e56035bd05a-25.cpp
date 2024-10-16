    unsigned char * SGeometry::serializeToWKB(size_t featureInd, int& wkbSize)
    {        
        unsigned char * ret = nullptr;
        int nc = 0; size_t sb = 0;

        // Points don't have node_count entry... only inspect and set node_counts if not a point
        if(this->getGeometryType() != POINT)
        {
            nc = node_counts[featureInd];
            sb = bound_list[featureInd];
        }

        // Serialization occurs differently depending on geometry
        // The memory requirements also differ between geometries
        switch(this->getGeometryType())
        {
            case POINT:
                wkbSize = 1 + 4 + this->touple_order * 8;
                ret = new uint8_t[wkbSize];
                inPlaceSerialize_Point(this, featureInd, ret);
                break;

            case LINE:
                wkbSize = 1 + 4 + 4 + this->touple_order * 8 * nc;
                ret = new uint8_t[wkbSize];
                inPlaceSerialize_LineString(this, nc, sb, ret);
                break;

            case POLYGON:
                // A polygon has:
                // 1 byte header
                // 4 byte Type
                // 4 byte ring count (1 (exterior)) [assume only exterior rings, otherwise multipolygon]
                // For each ring:
                // 4 byte point count, 8 byte double x point count x # dimension
                // (This is equivalent to requesting enough for all points and a point count header for each point)
                // (Or 8 byte double x node count x # dimension + 4 byte point count x part_count)

                // if interior ring, then assume that it will be a multipolygon (maybe future work?)
                wkbSize = 1 + 4 + 4 + 4 + this->touple_order * 8 * nc;
                ret = new uint8_t[wkbSize];
                inPlaceSerialize_PolygonExtOnly(this, nc, sb, ret);
                break;

            case MULTIPOINT:
                {
                    wkbSize = 1 + 4 + 4 + nc * (1 + 4 + this->touple_order * 8);
                    ret = new uint8_t[wkbSize];

                    void * worker = ret;
                    int8_t header = PLATFORM_HEADER;
                    uint32_t t = this->touple_order == 2 ? wkbMultiPoint :
                                 this->touple_order == 3 ? wkbMultiPoint25D : wkbNone;

                    if(t == wkbNone) throw SG_Exception_BadFeature();

                    // Add metadata
                    worker = memcpy_jump(worker, &header, 1);
                    worker = memcpy_jump(worker, &t, 4);
                    worker = memcpy_jump(worker, &nc, 4);

                    // Add points
                    for(int pts = 0; pts < nc; pts++)
                    {
                        worker = inPlaceSerialize_Point(this, static_cast<size_t>(sb + pts), worker);                                
                    }
                }

                break;

            case MULTILINE:
                {
                    int8_t header = PLATFORM_HEADER;
                    uint32_t t = this->touple_order == 2 ? wkbMultiLineString :
                                this->touple_order == 3 ? wkbMultiLineString25D : wkbNone;

                    if(t == wkbNone) throw SG_Exception_BadFeature();
                    int32_t lc = parts_count[featureInd];
                    size_t seek_begin = sb;
                    size_t pc_begin = pnc_bl[featureInd]; // initialize with first part count, list of part counts is contiguous    
                    wkbSize = 1 + 4 + 4;
                    std::vector<int> pnc;

                    // Build sub vector for part_node_counts
                    // + Calculate wkbSize
                    for(int itr = 0; itr < lc; itr++)
                    {
                        pnc.push_back(pnode_counts[pc_begin + itr]);    
                         wkbSize += this->touple_order * 8 * pnc[itr] + 1 + 4 + 4;
                    }

                
                    size_t cur_point = seek_begin;
                    size_t pcount = pnc.size();

                    // Allocate and set pointers
                    ret = new uint8_t[wkbSize];
                    void * worker = ret;

                    // Begin Writing
                    worker = memcpy_jump(worker, &header, 1);
                    worker = memcpy_jump(worker, &t, 4);
                    worker = memcpy_jump(worker, &pcount, 4);

                    for(size_t itr = 0; itr < pcount; itr++)
                    {
                            worker = inPlaceSerialize_LineString(this, pnc[itr], cur_point, worker);
                            cur_point = pnc[itr] + cur_point;
                    }
                }

                break;

            case MULTIPOLYGON:
                {
                    int8_t header = PLATFORM_HEADER;
                    uint32_t t = this->touple_order == 2 ? wkbMultiPolygon:
                                 this->touple_order == 3 ? wkbMultiPolygon25D: wkbNone;

                    if(t == wkbNone) throw SG_Exception_BadFeature();
                    bool noInteriors = this->int_rings.size() == 0 ? true : false;
                    int32_t rc = parts_count[featureInd];
                    size_t seek_begin = sb;
                    size_t pc_begin = pnc_bl[featureInd]; // initialize with first part count, list of part counts is contiguous        
                    wkbSize = 1 + 4 + 4;
                    std::vector<int> pnc;

                    // Build sub vector for part_node_counts
                    for(int itr = 0; itr < rc; itr++)
                    {
                        pnc.push_back(pnode_counts[pc_begin + itr]);    
                    }    

                    // Figure out each Polygon's space requirements
                    if(noInteriors)
                    {
                        for(int ss = 0; ss < rc; ss++)
                        {
                             wkbSize += 8 * this->touple_order * pnc[ss] + 1 + 4 + 4 + 4;
                        }
                    }

                    else
                    {
                        // Total space requirements for Polygons:
                        // (1 + 4 + 4) * number of Polygons
                        // 4 * number of Rings Total
                        // 8 * touple_order * number of Points
        

                        // Each ring collection corresponds to a polygon
                        wkbSize += (1 + 4 + 4) * poly_count[featureInd]; // (headers)

                        // Add header requirements for rings
                        wkbSize += 4 * parts_count[featureInd];

                        // Add requirements for number of points
                        wkbSize += 8 * this->touple_order * nc;
                    }

                    // Now allocate and serialize
                    ret = new uint8_t[wkbSize];
                
                    // Create Multipolygon headers
                    void * worker = (void*)ret;
                    worker = memcpy_jump(worker, &header, 1);
                    worker = memcpy_jump(worker, &t, 4);

                    if(noInteriors)
                    {
                        size_t cur_point = seek_begin;
                        size_t pcount = pnc.size();
                        worker = memcpy_jump(worker, &pcount, 4);

                        for(size_t itr = 0; itr < pcount; itr++)
                        {
                            worker = inPlaceSerialize_PolygonExtOnly(this, pnc[itr], cur_point, worker);
                            cur_point = pnc[itr] + cur_point;
                        }
                    }

                    else
                    {
                        int32_t polys = poly_count[featureInd];
                        worker = memcpy_jump(worker, &polys, 4);
    
                        size_t base = pnc_bl[featureInd]; // beginning of parts_count for this multigeometry
                        size_t seek = seek_begin; // beginning of node range for this multigeometry
                        size_t ir_base = base + 1;
                        int rc_m = 1; 

                        // has interior rings,
                        for(int32_t itr = 0; itr < polys; itr++)
                        {    
                            rc_m = 1;

                            // count how many parts belong to each Polygon        
                            while(ir_base < int_rings.size() && int_rings[ir_base])
                            {
                                rc_m++;
                                ir_base++;    
                            }

                            if(rc_m == 1) ir_base++;    // single polygon case

                            std::vector<int> poly_parts;

                            // Make part node count sub vector
                            for(int itr_2 = 0; itr_2 < rc_m; itr_2++)
                            {
                                poly_parts.push_back(pnode_counts[base + itr_2]);
                            }

                            worker = inPlaceSerialize_Polygon(this, poly_parts, rc_m, seek, worker);

                            // Update seek position
                            for(size_t itr_3 = 0; itr_3 < poly_parts.size(); itr_3++)
                            {
                                seek += poly_parts[itr_3];
                            }
                        }
                    }
                }    
                break;

                default:

                    throw SG_Exception_BadFeature();    
                    break;
        }

        return ret;
    }