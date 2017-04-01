#ifndef SPIMAGEPROC_H_
#define SPIMAGEPROC_H_
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

extern "C" {
#include "SPConfig.h"
#include "SPPoint.h"
}

namespace sp {

/**
 * A class which supports different image processing functionalites.
 */
class ImageProc {
private:
	const char* windowName = "Software Project CBIR";
	int pcaDim;
	int numOfImages;
	int numOfFeatures;
	cv::PCA pca;
	bool minimalGui;
	void initFromConfig(const SPConfig);
	void getImagesMat(std::vector<cv::Mat>&, const SPConfig);
	void getFeatures(std::vector<cv::Mat>&,
			cv::Mat&);
	void preprocess(const SPConfig config);
	void initPCAFromFile(const SPConfig config);
public:

	/**
	 * Creates a new object for the purpose of image processing based
	 * on the configuration file.
	 * @param config - the configuration file from which the object is created
	 */
	ImageProc(const SPConfig config);

	/**
	 * Returns an array of features for the image imagePath. All SPPoint elements
	 * will have the index given by index. The actual number of features extracted
	 * for this image will be stored in the pointer given by numOfFeats.
	 *
	 * @param imagePath - the target imagePath
	 * @param index - the index  of the image in the database
	 * @param numOfFeats - a pointer in which the actual number of feats extracted
	 * 					   will be stored
	 * @return
	 * An array of the actual features extracted. NULL is returned in case of
	 * an error.
	 */
	SPPoint** getImageFeatures(const char* imagePath,int index,int* numOfFeats);

	/**
	 *	Displays the image given by imagePath. Notice that this function works
	 *	only in MinimalGUI mode (otherwise a warnning message is printed).
	 *
	 *	@param imagePath - the path of the image to be displayed
	 */
	void showImage(const char* imagePath);
};

}
#endif
