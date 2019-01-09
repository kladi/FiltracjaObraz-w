#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string.h>

using namespace cv;

float CountDistance(int x, int y, int i, int j) {
	return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}

double GausFunction(float x, double sigma) {
	return exp(-(pow(x, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}

void applyBilateralFilterWithColors(Mat source, Mat &filteredImage2, Mat &filteredImage3, Mat &filteredImage4, int x, int y, int diameter, double sigmaI, double sigmaS) {
	//L, a, b
	double iFiltered[3];
	iFiltered[0] = 0;
	iFiltered[1] = 0;
	iFiltered[2] = 0;

	double wP = 0;
	int neighborX = 0;
	int neighborY = 0;
	int halfDiameter = diameter / 2;

	for (int i = 0; i < diameter; i++) {
		for (int j = 0; j < diameter; j++) {
			neighborX = x - (halfDiameter - i);
			neighborY = y - (halfDiameter - j);
			double gi = GausFunction(
				sqrt(pow(source.at<cv::Vec3b>(neighborX, neighborY)[0] - source.at<cv::Vec3b>(x, y)[0], 2) +
					pow(source.at<cv::Vec3b>(neighborX, neighborY)[1] - source.at<cv::Vec3b>(x, y)[1], 2) +
					pow(source.at<cv::Vec3b>(neighborX, neighborY)[2] - source.at<cv::Vec3b>(x, y)[2], 2)),
				sigmaI);
			double gs = GausFunction(CountDistance(x, y, neighborX, neighborY), sigmaS);
			double w = gi * gs;
			iFiltered[0] = iFiltered[0] + filteredImage2.at<uchar>(neighborX, neighborY) * w;
			iFiltered[1] = iFiltered[1] + filteredImage3.at<uchar>(neighborX, neighborY) * w;
			iFiltered[2] = iFiltered[2] + filteredImage4.at<uchar>(neighborX, neighborY) * w;
			wP = wP + w;
		}
	}
	filteredImage2.at<uchar>(x, y) = iFiltered[0] / wP;
	filteredImage3.at<uchar>(x, y) = iFiltered[1] / wP;
	filteredImage4.at<uchar>(x, y) = iFiltered[2] / wP;
}

void bilateralFilterWithColor(Mat source, Mat &colorRgbFiltered, int diameter, double sigmaI, double sigmaS) {
	std::cout << "Init function: bilateralFilterWithColor " << std::endl;

	Mat filteredImage = source;
	int width = source.cols;
	int height = source.rows;
	int margines = ceil(diameter / 2);

	Mat sourceLab;
	cvtColor(source, sourceLab, cv::COLOR_RGB2Lab);

	std::vector<Mat> FilteredChannels(3);
	split(sourceLab, FilteredChannels);

	std::vector<Mat> SourceLabChannels(3);
	split(sourceLab, SourceLabChannels);
	std::vector<Mat> newChannels2 = { SourceLabChannels[0], SourceLabChannels[1], SourceLabChannels[2] };
	imwrite("1SourceLabChannels1.png", SourceLabChannels[0]);
	imwrite("1SourceLabChannels2.png", SourceLabChannels[1]);
	imwrite("1SourceLabChannels3.png", SourceLabChannels[2]);

	for (int i = margines; i < height - margines; i++) {
		for (int j = margines; j < width - margines; j++) {
			applyBilateralFilterWithColors(sourceLab, FilteredChannels.at(0), FilteredChannels.at(1), FilteredChannels.at(2), i, j, diameter, sigmaI, sigmaS);
		}
	}

	imwrite("2FiltrowanyKanal1.png", FilteredChannels.at(0));
	imwrite("2FiltrowanyKanal2.png", FilteredChannels.at(1));
	imwrite("2FiltrowanyKanal3.png", FilteredChannels.at(2));

	std::vector<Mat> newChannels3 = { FilteredChannels[0], FilteredChannels[1], FilteredChannels[2] };
	Mat mergedLabFiltered;
	merge(newChannels3, mergedLabFiltered);
	//Mat colorRgbFiltered;
	cvtColor(mergedLabFiltered, colorRgbFiltered, COLOR_Lab2RGB);
	imwrite("4FilteredImage.png", colorRgbFiltered);
}

int main() {
	std::cout << "Init " << std::endl;
	Mat src;
	std::string fileName = "";
	std::cout << "Picture file name: ";
	std::cin >> fileName;

	src = imread(fileName, CV_LOAD_IMAGE_COLOR);

	if (!src.data)
	{
		printf("No image data \n");
		std::cout << "No image data" << std::endl;
		return -1;
	}

	int numberOfIterations = 5;
	for (int i = 0; i < numberOfIterations; i++) {
		Mat filteredImage;
		bilateralFilterWithColor(src, filteredImage, 3, 12.0, 16.0);
		std::string fileName = "4FilteredImageIteration";
		fileName += std::to_string(i);
		fileName += ".png";
		const char * tempElement = fileName.c_str();
		imwrite(tempElement, filteredImage);
		std::cout << " Saved filtered picture to file:  " << fileName << std::endl;
		src = filteredImage;
	}

	return 0;
}


