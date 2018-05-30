#include "utility.h"

utility::utility()
{
}

QString utility::createImage(int imgWidth, int imgHeight)
{
    //create an image ( 3 channels, 16 bit image depth, 650 high, 600 wide, (0, 50000, 50000) assigned for Blue, Green and Red plane respectively. )
    Mat img(imgHeight, imgWidth, CV_16UC3, Scalar(0, 50000 ,50000));

    //check whether the image is loaded or not
    if (img.empty())
    {
        cout << "ERROR : Image cannot be loaded..!!" << endl;
    }

    //vector that stores the compression parameters of the image
    vector<int> compression_params;
    //specify the compression technique
    #if USECV3 == 1
        compression_params.push_back(IMWRITE_JPEG_QUALITY);
    #endif


    #if USECV3 == 0
        compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    #endif
    //specify the compression quality
    compression_params.push_back(98);

    //file string
    QString imageName = "/home/goddard/Projects/vision/images/TestImage.jpg";

    //write the image to file
    bool bSuccess = imwrite(imageName.toStdString(), img, compression_params);

    if ( !bSuccess )
    {
        cout << "ERROR : Failed to save the image" << endl;
        return 0;
    }

    return imageName;
    //namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
    //imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window

    //waitKey(0);  //wait for a keypress

    //destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
}

