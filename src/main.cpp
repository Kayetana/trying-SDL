#include "engine/Engine.hpp"

#include <iostream>
#include <cmath>
#include <map>

#include <fstream>


int main() {
    Engine engine("/home/user/TP/SDL2Test/src/engine/config.txt");
    engine.run();
    return 0;
}
