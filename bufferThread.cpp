#include "bufferThread.h"

bufferThread::bufferThread()
{
    // Initialize variables(s)
    nArrived=0;
    doSync=false;
}

void bufferThread::add(int deviceNumber, tbuffer<Mat>* imageBuffer, bool sync)
{
    // Device stream is to be synchronized
    if(sync)
    {
        mutex.lock();
        syncSet.insert(deviceNumber);
        mutex.unlock();
    }
    // Add image buffer to map
    imageBufferMap[deviceNumber]=imageBuffer;
}

tbuffer<Mat>* bufferThread::getByDeviceNumber(int deviceNumber)
{
    return imageBufferMap[deviceNumber];
}

void bufferThread::removeByDeviceNumber(int deviceNumber)
{
    // Remove buffer for device from imageBufferMap
    imageBufferMap.remove(deviceNumber);

    // Also remove from syncSet (if present)
    mutex.lock();
    if(syncSet.contains(deviceNumber))
    {
        syncSet.remove(deviceNumber);
        wc.wakeAll();
    }
    mutex.unlock();
}

void bufferThread::sync(int deviceNumber)
{
    // Only perform sync if enabled for specified device/stream
    mutex.lock();
    if(syncSet.contains(deviceNumber))
    {
        // Increment arrived count
        nArrived++;
        // We are the last to arrive: wake all waiting threads
        if(doSync && (nArrived==syncSet.size()))
            wc.wakeAll();
        // Still waiting for other streams to arrive: wait
        else
            wc.wait(&mutex);
        // Decrement arrived count
        nArrived--;
    }
    mutex.unlock();
}

void bufferThread::wakeAll()
{
    QMutexLocker locker(&mutex);
    wc.wakeAll();
}

void bufferThread::setSyncEnabled(bool enable)
{
    doSync=enable;
}

bool bufferThread::isSyncEnabledForDeviceNumber(int deviceNumber)
{
    return syncSet.contains(deviceNumber);
}

bool bufferThread::getSyncEnabled()
{
    return doSync;
}

bool bufferThread::containsImageBufferForDeviceNumber(int deviceNumber)
{
    return imageBufferMap.contains(deviceNumber);
}
