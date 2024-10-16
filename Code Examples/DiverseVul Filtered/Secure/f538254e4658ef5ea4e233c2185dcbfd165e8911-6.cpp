template <class T> void pop_front(std::vector<T>& vec)
{
  for (int i=1;i<vec.size();i++)
    vec[i-1] = vec[i];

  vec.pop_back();
}