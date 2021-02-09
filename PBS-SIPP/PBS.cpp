#include "PBS.h"

PBS::PBS(Instance& instance): sipp(instance), instance(instance)
{

}

void PBS::run()
{
    //
    int i = 0; // i is the index of the agent that we want to replan
    ReservationTable rt(instance.getNumOfVertices());
    // add paths of higher-priority agents to rt
    Path path = sipp.run(i, rt);
    //
}

void PBS::writeToFile(const string& file_name)
{

}