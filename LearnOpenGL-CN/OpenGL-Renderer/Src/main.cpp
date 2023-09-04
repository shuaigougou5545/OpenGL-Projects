#include <memory>
#include "Engine.h"


int main()
{
    std::shared_ptr<Engine> engine = std::make_shared<Engine>();
    
    engine->startEngine();
    engine->run();
    engine->shutdownEngine();
    
    return 0;
}
