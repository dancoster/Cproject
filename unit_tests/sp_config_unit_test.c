#include "unit_test_util.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../SPConfig.h"
#include <stdlib.h>

//checks if there is a config file
bool noConfigFile(){
	SP_CONFIG_MSG msg;
	ASSERT_TRUE(spConfigCreate("bla.config", &msg) == NULL);
	ASSERT_TRUE(msg==SP_CONFIG_CANNOT_OPEN_FILE);
	return true;
}

//checks if there are no values in parameters
bool configFileNoArgs(){
	SP_CONFIG_MSG msg;
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_dir.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_prefix.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_suffix.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_num_images.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_INVALID_STRING);

	return true;
}

//checks if there are no parameters
bool configFileMissingParameterLines(){
	SP_CONFIG_MSG msg;
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_prefix_line.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_PREFIX);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_suffix_line.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_SUFFIX);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_dir_line.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_DIR);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_no_num_images_line.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_NUM_IMAGES);
	return true;
}

//checks if there are invalid parameters
bool configFileInvalidArgs(){
	SP_CONFIG_MSG msg;
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_invalid_dir.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_DIR);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_invalid_prefix.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_PREFIX);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_invalid_suffix.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_SUFFIX);
	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_invalid_num_images.config",&msg)==NULL);
	ASSERT_TRUE(msg==SP_CONFIG_MISSING_NUM_IMAGES);
//	ASSERT_TRUE(spConfigCreate("./unit_tests/configs/config_invalid_num_features.config",&msg)==NULL);
//	ASSERT_TRUE(msg==SP_CONFIG_MISSING_NUM_IMAGES);
	return true;
}

//checks if the config set the true values
bool configFilesSetValues(){
	SP_CONFIG_MSG msg;
	SPConfig config;
	int num;
	bool bool1;
	char char1[1024*4];


	config = spConfigCreate("./unit_tests/configs/config_valid.config",&msg);
	num = spConfigGetNumOfFeatures(config,&msg);
	ASSERT_TRUE(num==40);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	bool1 = spConfigIsExtractionMode(config,&msg);
	ASSERT_TRUE(bool1==false);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	bool1 = spConfigMinimalGui(config,&msg);
	ASSERT_TRUE(bool1==true);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	num = spConfigGetNumOfImages(config,&msg);
	ASSERT_TRUE(num==12);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	num = spConfigGetPCADim(config,&msg);
	ASSERT_TRUE(num==13);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	num = spConfigGetNumOfSimilarImages(config,&msg);
	ASSERT_TRUE(num==5);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);
	return true;

	msg = spConfigGetImagePath(char1,config,0);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);
	num = strcmp(char1,"./images/test1/pref0.png");
	ASSERT_TRUE(num==0);

	msg = spConfigGetPCAPath(char1,config);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);
	num = strcmp(char1,"./images/test1/testPca.name");
	ASSERT_TRUE(num==0);

	return true;
	}

bool configFilesDefultValues(){
	SP_CONFIG_MSG msg;
	SPConfig config;
	int num;
	bool bool1;
	char char1[1024*4];


	config = spConfigCreate("./unit_tests/configs/config_deafult.config",&msg);
	num = spConfigGetNumOfFeatures(config,&msg);
	ASSERT_TRUE(num==100);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	bool1 = spConfigIsExtractionMode(config,&msg);
	ASSERT_TRUE(bool1==true);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	bool1 = spConfigMinimalGui(config,&msg);
	ASSERT_TRUE(bool1==false);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	num = spConfigGetPCADim(config,&msg);
	ASSERT_TRUE(num==20);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);


	msg = spConfigGetPCAPath(char1,config);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);
	num = strcmp(char1,"./images/pca.yml");
	ASSERT_TRUE(num==0);

	return true;
	}

int main(){
	RUN_TEST(noConfigFile);
	printf("*********************************************\n");
	RUN_TEST(configFileNoArgs);
	printf("*********************************************\n");
	RUN_TEST(configFileMissingParameterLines);
	printf("*********************************************\n");
	RUN_TEST(configFileInvalidArgs);
	printf("*********************************************\n");
	RUN_TEST(configFilesSetValues);
	printf("*********************************************\n");
	RUN_TEST(configFilesDefultValues);
	printf("*********************************************\n");

	printf("test ok!");
	return 0;
}

