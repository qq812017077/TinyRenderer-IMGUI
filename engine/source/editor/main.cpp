
#include <unordered_set>
#include <memory>
#include "Engine.h"
#include "Editor.h"

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
  auto & engine = TinyEngine::Engine::Get();
  auto & editor = TinyEngine::Editor::Get();
  engine.Initialize();
  editor.Initialize(&(engine));
  editor.Run();
  editor.Clear();
  engine.ShutdownEngine();
  return 0;
  // App app{};
  // return app.Run();
}