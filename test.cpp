// unordered_map::size
#include <iostream>
#include <string>
#include <unordered_map>

int main ()
{
  std::unordered_map<std::string,double> mymap;

  std::cout << "mymap.size() is " << mymap.size() << std::endl;

  return 0;
}