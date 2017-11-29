#include "objectdetection.h"

objectDetection::objectDetection()
{


}

void objectDetection::intruderAlarm(cv::Mat& a, cv::Mat& b)
{
    // Blur images to reduce noise
    cv::Mat a_blurred, b_blurred;
    cv::blur(a, a_blurred, cv::Size(4,4));
    cv::blur(b, b_blurred, cv::Size(4,4));

    // Get absolute difference image
    cv::Mat c;
    cv::absdiff(b_blurred, a_blurred, c);

    // Split image to each channels
    std::vector<cv::Mat> channels;
    cv::split(c, channels);

    // Apply threshold to each channel and combine the results
    cv::Mat d = cv::Mat::zeros(c.size(), CV_8UC1);
    for (int i = 0; i < channels.size(); i++)
    {
        cv::Mat thresh;
        #if USECV3 == 1
            cv::threshold(channels[i], thresh, 45, 255, THRESH_BINARY);
        #endif

        #if USECV3 == 0
            cv::threshold(channels[i], thresh, 45, 255, CV_THRESH_BINARY);
        #endif
        d |= thresh;
    }

    // Perform morphological close operation to filling in the gaps
    cv::Mat kernel, e;
    cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10,10));
    cv::morphologyEx(d, e, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1), 5);

    // Find all contours
    std::vector<std::vector<cv::Point> > contours;

    #if USECV3 == 1
        cv::findContours(e.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    #endif

    #if USECV3 == 0
        cv::findContours(e.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    #endif

    // Select only large enough contours
    std::vector<std::vector<cv::Point> > intruders;
    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area > 1000 && area < 70000)
            intruders.push_back(contours[i]);
    }

    // Use the filtered blobs above to create a mask image to
    // extract the foreground object
    cv::Mat mask = cv::Mat::zeros(a.size(), CV_8UC3);

    #if USECV3 == 1
        cv::drawContours(mask, intruders, -1, QColor(255, 255, 255, 255).value(), -1); //QColor(255, 255, 255, 127)
    #endif

    #if USECV3 == 0
        cv::drawContours(mask, intruders, -1, CV_RGB(255,255,255), -1);
    #endif
    // Highlight the foreground object by darken the rest of the image
    if (intruders.size())
    {
        a = (a/4 & ~mask) + (a & mask);
        #if USECV3 == 1
            cv::drawContours(a, intruders, -1, QColor(255, 255, 255, 255).value(), 2);
        #endif

        #if USECV3 == 0
            cv::drawContours(a, intruders, -1, CV_RGB(255,255,255), 2);
        #endif
    }
}
