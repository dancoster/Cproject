#include <cstdlib>
#include <cassert>
#include <cstring>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <cstdio>
#include "SPImageProc.h"
extern "C" {
#include "SPLogger.h"
}

using namespace cv;
using namespace std;

#define PCA_MEAN_STR "mean"
#define PCA_EIGEN_VEC_STR "e_vectors"
#define PCA_EIGEN_VAL_STR "e_values"
#define STRING_LENGTH 1024
#define WARNING_MSG_LENGTH 2048

#define GENERAL_ERROR_MSG "An error occurred"
#define PCA_DIM_ERROR_MSG "PCA dimension couldn't be resolved"
#define PCA_FILE_NOT_EXIST "PCA file doesn't exist"
#define PCA_FILE_NOT_RESOLVED "PCA filename couldn't be resolved"
#define NUM_OF_IMAGES_ERROR "Number of images couldn't be resolved"
#define NUM_OF_FEATS_ERROR "Number of features couldn't be resolved"
#define MINIMAL_GUI_ERROR "Minimal GUI mode couldn't be resolved"
#define IMAGE_PATH_ERROR "Image path couldn't be resolved"
#define IMAGE_NOT_EXIST_MSG ": Images doesn't exist"
#define MINIMAL_GUI_NOT_SET_WARNING "Cannot display images in non-Minimal-GUI mode"
#define ALLOC_ERROR_MSG "Allocation error"
#define INVALID_ARG_ERROR "Invalid arguments"

void sp::ImageProc::initFromConfig(const SPConfig config) {
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	pcaDim = spConfigGetPCADim(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(PCA_DIM_ERROR_MSG, __FILE__, __func__, __LINE__);
		throw Exception();
	}
	numOfImages = spConfigGetNumOfImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
		throw Exception();
	}
	numOfFeatures = spConfigGetNumOfFeatures(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(NUM_OF_FEATS_ERROR, __FILE__, __func__, __LINE__);
		throw Exception();
	}
	minimalGui = spConfigMinimalGui(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(MINIMAL_GUI_ERROR, __FILE__, __func__, __LINE__);
		throw Exception();
	}
}

void sp::ImageProc::getImagesMat(vector<Mat>& images, const SPConfig config) {
	char warningMSG[WARNING_MSG_LENGTH] = { '\0' };
	for (int i = 0; i < numOfImages; i++) {
		char imagePath[STRING_LENGTH + 1] = { '\0' };
		if (spConfigGetImagePath(imagePath, config, i) != SP_CONFIG_SUCCESS) {
			spLoggerPrintError(IMAGE_PATH_ERROR, __FILE__, __func__, __LINE__);
			throw Exception();
		}
		Mat img = imread(imagePath, IMREAD_GRAYSCALE);
		if (img.empty()) {
			sprintf(warningMSG, "%s %s", imagePath, IMAGE_NOT_EXIST_MSG);
			spLoggerPrintWarning(warningMSG, __FILE__, __func__, __LINE__);
			continue;
		}
		images.push_back(img);
	}
}

void sp::ImageProc::getFeatures(vector<Mat>& images, Mat& features) {
	//To store the keypoints that will be extracted by SIFT
	vector<KeyPoint> keypoints;
	//To store the SIFT descriptor of current image
	Mat descriptor;
	//To store all the descriptors that are extracted from all the images.

	//The SIFT feature extractor and descriptor
	Ptr<xfeatures2d::SiftDescriptorExtractor> detector =
			xfeatures2d::SIFT::create(numOfFeatures);

	//feature descriptors and build the vocabulary
	for (int i = 0; i < static_cast<int>(images.size()); i++) {
		//detect feature points
		detector->detect(images[i], keypoints);
		//compute the descriptors for each keypoint
		detector->compute(images[i], keypoints, descriptor);
		//put the all feature descriptors in a single Mat object
		features.push_back(descriptor);
	}
}

void sp::ImageProc::preprocess(const SPConfig config) {
	try {
		vector<Mat> images;
		Mat features;
		char pcaPath[STRING_LENGTH + 1] = { '\0' };
		getImagesMat(images, config);
		getFeatures(images, features);
		pca = PCA(features, Mat(), CV_PCA_DATA_AS_ROW, pcaDim);
		if (spConfigGetPCAPath(pcaPath, config) != SP_CONFIG_SUCCESS) {
			spLoggerPrintError(PCA_FILE_NOT_RESOLVED, __FILE__, __func__,
			__LINE__);
			throw Exception();
		}
		FileStorage fs(pcaPath, FileStorage::WRITE);
		fs << PCA_EIGEN_VEC_STR << pca.eigenvectors;
		fs << PCA_EIGEN_VAL_STR << pca.eigenvalues;
		fs << PCA_MEAN_STR << pca.mean;
		fs.release();
	} catch (...) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		throw Exception();
	}
}

void sp::ImageProc::initPCAFromFile(const SPConfig config) {
	if (!config) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		throw Exception();
	}
	char pcaFilename[STRING_LENGTH + 1] = { '\0' };
	if (spConfigGetPCAPath(pcaFilename, config) != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(PCA_FILE_NOT_RESOLVED, __FILE__, __func__, __LINE__);
		throw Exception();
	}
	FileStorage fs(pcaFilename, FileStorage::READ);
	if (!fs.isOpened()) {
		spLoggerPrintError(PCA_FILE_NOT_EXIST, __FILE__, __func__, __LINE__);
		throw Exception();
	}
	fs[PCA_EIGEN_VEC_STR] >> pca.eigenvectors;
	fs[PCA_EIGEN_VAL_STR] >> pca.eigenvalues;
	fs[PCA_MEAN_STR] >> pca.mean;
	fs.release();
}

sp::ImageProc::ImageProc(const SPConfig config) {
	try {
		if (!config) {
			spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
			throw Exception();
		}
		SP_CONFIG_MSG msg;
		bool preprocMode = false;
		initFromConfig(config);
		if ((preprocMode = spConfigIsExtractionMode(config, &msg))) {
			preprocess(config);
		} else {
			initPCAFromFile(config);
		}
	} catch (...) {
		spLoggerPrintError(GENERAL_ERROR_MSG, __FILE__, __func__, __LINE__);
		throw Exception();
	}
}

SPPoint** sp::ImageProc::getImageFeatures(const char* imagePath, int index,
		int* numOfFeats) {
	vector<KeyPoint> keypoints;
	Mat descriptor, img, points;
	double* pcaSift = NULL;
	char errorMSG[STRING_LENGTH * 2];
	Ptr<xfeatures2d::SiftDescriptorExtractor> detector;
	if (!imagePath || !numOfFeats) {
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	img = imread(imagePath, IMREAD_GRAYSCALE);
	if (img.empty()) {
		sprintf(errorMSG, "%s %s", imagePath, IMAGE_NOT_EXIST_MSG);
		spLoggerPrintError(errorMSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	detector = xfeatures2d::SIFT::create(numOfFeatures);
	detector->detect(img, keypoints);
	detector->compute(img, keypoints, descriptor);
	points = pca.project(descriptor);
	pcaSift = (double*) malloc(sizeof(double) * pcaDim);
	if (!pcaSift) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	*numOfFeats = points.rows;
	SPPoint** resPoints = (SPPoint**) malloc(sizeof(*resPoints) * points.rows);
	if (!resPoints) {
		free(pcaSift);
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	for (int i = 0; i < points.rows; i++) {
		for (int j = 0; j < points.cols; j++) {
			pcaSift[j] = (double) points.at<float>(i, j);
		}
		resPoints[i] = spPointCreate(pcaSift, pcaDim, index);
	}
	free(pcaSift);
	return resPoints;
}

void sp::ImageProc::showImage(const char* imgPath) {
	if (minimalGui) {
		Mat img = imread(imgPath, cv::IMREAD_COLOR);
		if (img.empty()) {
			spLoggerPrintWarning(IMAGE_NOT_EXIST_MSG, __FILE__, __func__,
			__LINE__);
			return;
		}
		imshow(windowName, img);
		waitKey(0);
		destroyAllWindows();
	} else {
		spLoggerPrintWarning(MINIMAL_GUI_NOT_SET_WARNING, __FILE__, __func__,
		__LINE__);
	}

}

