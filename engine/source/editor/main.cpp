
#include <unordered_set>
#include <memory>
#include "Engine.h"
#include "Editor.h"

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
}