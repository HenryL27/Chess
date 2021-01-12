#include "Engine.hh"
#include <fstream>

class Learner : public Engine {
    public:
        Learner(std::ifstream);
        virtual void write(std::ofstream) = 0;
}