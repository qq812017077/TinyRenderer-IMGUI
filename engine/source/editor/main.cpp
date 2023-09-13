
#include "App.h"
#include <unordered_set>
#include <memory>
class A
{
public:
  A()
  {
    std::cout << "A()" << std::endl;
  }
  A(const A&)
  {
    std::cout << "A(const A&)" << std::endl;
  }
  A(A&&)
  {
    std::cout << "A(A&&)" << std::endl;
  }
  ~A()
  {
    std::cout << "~A()" << std::endl;
  }
  int var = 1;
};


// Main code
int main(int, char**)
{
  App app{};
  return app.Run();
}