#include "EngineException.h"
#include "App.h"
// Main code
int main(int, char**)
{
    try
    {
        App app{};
        return app.Run();
    }
    catch(const EngineException& e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch(const std::exception& e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch(...)
    {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    return 0;
}