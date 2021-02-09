#pragma once
#include "Instance.h"



class SIPP {
public:
    SIPP(Instance& instance);
    Path run(int agent, const ReservationTable& rt);

private:
    Instance& instance;
};

