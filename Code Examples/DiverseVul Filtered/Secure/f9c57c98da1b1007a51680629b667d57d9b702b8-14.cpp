void emitFlightTimes()
{
  uint64_t totals = countLessThan(flightTimes.size());
  unsigned int limits[]={1, 2, 3, 4, 5, 10, 20, 30, 40, 50, 100, 200, 500, 1000, (unsigned int) flightTimes.size()};
  uint64_t sofar=0;
  cout.setf(std::ios::fixed);
  cout.precision(2);
  for(unsigned int i =0 ; i < sizeof(limits)/sizeof(limits[0]); ++i) {
    if(limits[i]!=flightTimes.size())
      cout<<"Within "<<limits[i]<<" msec: ";
    else 
      cout<<"Beyond "<<limits[i]-2<<" msec: ";
    uint64_t here = countLessThan(limits[i]);
    cout<<100.0*here/totals<<"% ("<<100.0*(here-sofar)/totals<<"%)"<<endl;
    sofar=here;
    
  }
}