#include <memory>
#include "Engine.h"


int main()
{
    std::shared_ptr<rd::Engine> engine = std::make_shared<rd::Engine>();
    
    engine->startEngine();
    engine->run();
    engine->shutdownEngine();
    
    return 0;
}
