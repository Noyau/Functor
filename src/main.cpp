// Crappy test 'cause I don't have time...
// Well... Actually, I do have time but...
// You know what? Let say I'll do proper
// tests soon! (one day) (maybe)
// (I'm a liar.)

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Functor.h"

#define Assert(condition)\
{\
    std::ostringstream ss;\
    ss << "\" " << #condition << " \" => ";\
    if ((condition))\
    {\
        std::cout << ss.str() << "True" << std::endl;\
    }\
    else\
    {\
        std::cerr << ss.str() << "False" << std::endl;\
        std::cerr << __FILE__ << "(" << __LINE__ << ")" << std::endl;\
        exit(EXIT_FAILURE);\
    }\
}

static int Add(int a, int b) { return a + b; }
static int Mul(int a, int b) { return a * b; }

struct Math
{
    int Add(int a, int b) { return a + b; }
    int Mul(int a, int b) { return a * b; }
};

int main(int argc, char* argv[])
{
    static constexpr int arg1{ 49999 };
    static constexpr int arg2{ 17761 };

    {
        auto add = MakeFunctor(&Add);
        Assert(add(arg1, arg2) == Add(arg1, arg2));

        auto mul = MakeFunctor(&Mul);
        Assert(mul(arg1, arg2) == Mul(arg1, arg2));
    }

    {
        Math m;

        auto add = MakeFunctor(m, &Math::Add);
        Assert(add(arg1, arg2) == m.Add(arg1, arg2));

        auto mul = MakeFunctor(m, &Math::Mul);
        Assert(mul(arg1, arg2) == m.Mul(arg1, arg2));
    }

    return EXIT_SUCCESS;
}