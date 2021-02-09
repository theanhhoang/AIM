#pragma once
#include "SIPP.h"

class PBS
{
public:
    PBS(Instance& instance);
    void run();
    void writeToFile(const string& file_name);

private:
    SIPP sipp;
    Instance& instance;
};

