//
//
//

#ifndef _TIMING_H
#define _TIMING_H

struct TimingData
{
    //The current render frame.
    unsigned frameNumber;
     //The timestamp when the last frame ended.
    unsigned lastFrameTimestamp;
     // The duration of the last frame.
    unsigned lastFrameDuration;
    // The clockstamp of the end of the last frame.
    unsigned long lastFrameClockstamp;
     //The duration of the last frame in clock ticks.
    unsigned long lastFrameClockTicks;
     //Keeps track of whether the rendering is paused.
    bool isPaused;
     // This is a recency weighted average of the frame time, calculated
     // from frame durations.
    float averageFrameDuration;
     // The reciprocal of the average frame duration giving the mean
     // fps over a recency weighted average.
    float fps;
     //Gets the global timing data object.
    static TimingData* get();
    //Updates the timing system, should be called once per frame.
    static void update();
    //Initialises the frame information system.
    static void init();
    static void deinit();

     //Gets the global system time, in the best resolution possible(ms)
     //Timing is in milliseconds.
    static unsigned getTime();

    // Gets the clock ticks since process start.
    static unsigned long getClock();

    // These are private to stop instances being created
private:
    TimingData(){};
    TimingData(const TimingData &);
    TimingData& operator=(const TimingData &);
};

#endif //_TIMING_H
