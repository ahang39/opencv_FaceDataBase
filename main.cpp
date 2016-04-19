#include "opencv2/core/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

static void read_csv(const string& filename, vector<Mat>& images, vector<string>& paths, vector<int>& labels, char separator = ',') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			Mat temp = imread(path, 0);
			images.push_back(temp);
			paths.push_back(path);
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

int main(int argc, const char *argv[]) {
	string fn_haar = "C:\\Program Files\\opencv\\install\\etc\\haarcascades\\haarcascade_frontalface_default.xml";
	string fn_csv = "C:\\Users\\ahang\\Pictures\\opencv\\faces\\facebase.csv";
	vector<Mat> images;
	vector<string> paths;
	vector<int> labels;
	try {
		read_csv(fn_csv, images, paths, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		exit(1);
	}
	CascadeClassifier haar_cascade;
	haar_cascade.load(fn_haar);
	for (int i = 0; i<images.size(); i++)
	{
		Mat original;
		resize(images[i], original, Size((int)images[i].cols / 4, (int)images[i].rows / 4));
		imshow("original", original);
		Mat gray = original;
		if (original.channels() == 3)
			cvtColor(original, gray, CV_BGR2GRAY);
		//imshow("gray", gray); 
		vector< Rect_<int>> faces;
		haar_cascade.detectMultiScale(gray, faces);
		cout << faces.size() << endl;
		int max = 0;
		int index = 0;
		for (int j = 0; j < faces.size(); j++)
		{
			if (faces[j].area() > max)
			{
				max = faces[j].area();
				index = j;
			}
		}
		if (faces.size() >= 1)
		{
			int length = min(faces[index].height, faces[index].width);
			Mat ROI = gray(Rect((int)(faces[index].x+0.15*length) , (int)(faces[index].y + 0.15*length),(int)(0.7*length), (int)(0.7*length)));
			//imshow("resize", ROI);
			resize(ROI, ROI, Size(100, 100), 1.0, 1.0, INTER_CUBIC);
			imshow("ROI", ROI);
			char temp[4];
			_itoa(labels[i], temp, 10);
			string stemp = temp;
			string path = "C:\\Users\\ahang\\Pictures\\opencv\\faces\\temp\\" + stemp + "_";
			_itoa(i, temp, 10);
			stemp = temp;
			path = path + stemp + ".jpg";
			imwrite(path, ROI);
		}
		waitKey(1);
	}
	return 0;
}