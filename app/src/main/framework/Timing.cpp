//
//
//

#include "Timing.h"
#include <stdlib.h>
#include <sys/time.h>

#define TIMING_UNIX	1

// Internal time and clock access functions
unsigned systemTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (unsigned long)tv.tv_sec * 1000 + tv.tv_usec/1000;

}

unsigned TimingData::getTime()
{
    return systemTime();
}


unsigned long TimingData::getClock()
{

    struct timeval tv;
    gettimeofday(&tv, 0);
    return (unsigned long)tv.tv_sec * 1000 + tv.tv_usec/1000;

}
// Holds the global frame time that is passed around
//static TimingData *timingData = NULL;
// Retrieves the global frame info instance
TimingData* TimingData::get()
{
    static TimingData timingData;
    return &timingData;
}

void TimingData::update()
{
    TimingData* timingData = TimingData::get();
    if (!timingData) return;

    // Advance the frame number.
    if (!timingData->isPaused)
    {
        timingData->frameNumber++;
    }

    // Update the timing information.
    unsigned thisTime = systemTime();
    timingData->lastFrameDuration = thisTime -
                                    timingData->lastFrameTimestamp;
    timingData->lastFrameTimestamp = thisTime;

    // Update the tick information.
    unsigned long thisClock = getClock();
    timingData->lastFrameClockTicks =
            thisClock - timingData->lastFrameClockstamp;
    timingData->lastFrameClockstamp = thisClock;

    // Update the frame rate if we are able to.
    if (timingData->frameNumber > 1) {
        if (timingData->averageFrameDuration <= 0)
        {
            timingData->averageFrameDuration =
                    (float)timingData->lastFrameDuration;
        }
        else
        {
            // Over 100 frames.
            timingData->averageFrameDuration *= 0.99;
            timingData->averageFrameDuration +=
                    0.01 * (float)timingData->lastFrameDuration;

            // Invert to get FPS
            timingData->fps =
                    (float)(1000.0/timingData->averageFrameDuration);
        }
    }
}

void TimingData::init()
{
    TimingData* timingData = TimingData::get();

    // Set up the frame info structure.
    timingData->frameNumber = 0;

    timingData->lastFrameTimestamp = systemTime();
    timingData->lastFrameDuration = 0;

    timingData->lastFrameClockstamp = getClock();
    timingData->lastFrameClockTicks = 0;

    timingData->isPaused = false;

    timingData->averageFrameDuration = 0;
    timingData->fps = 0;
}

void TimingData::deinit()
{

}
