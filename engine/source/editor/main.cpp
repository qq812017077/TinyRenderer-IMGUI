
#include "App.h"
#include <unordered_set>
struct Info2 {
  uint8_t a;
  uint16_t b;
  uint8_t c;
};


// Main code
int main(int, char**)
{
    // int && a = 10;
    App app{};
    return app.Run();
}