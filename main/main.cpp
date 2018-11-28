#include <iostream>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "ProtoNN.h"

static const char *TAG = "EdgeML";

using namespace EdgeML;

extern "C" void app_main(void)
{
  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };

  // Use settings defined above to initialize and mount SPIFFS filesystem.
  // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
          ESP_LOGE(TAG, "Failed to mount or format filesystem");
      } else if (ret == ESP_ERR_NOT_FOUND) {
          ESP_LOGE(TAG, "Failed to find SPIFFS partition");
      } else {
          ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
      }
      return;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total, &used);
  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
  } else {
      ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }

  clock_t tStart = clock();

  assert(sizeof(MKL_INT) == 8 && "need large enough indices to store matrices");
  assert(sizeof(MKL_INT) == sizeof(Eigen::Index) && "MKL BLAS routines are called directly on data of an Eigen matrix. Hence, the index sizes should match.");

  std::vector<std::string> arguments = {"main", "-I", "/spiffs/test.txt", "-M", "/spiffs/model", "-n",
      "/spiffs/minMaxParams", "-O", "/spiffs", "-F", "0","-e","200"};

  std::vector<char*> argv;
  for (const auto& arg : arguments){
    argv.push_back((char*)arg.data());
  }
  argv.push_back(nullptr);

  ProtoNN::ProtoNNPredictor predictor(argv.size() - 1, (const char**)argv.data());
  EdgeML::ResultStruct res;

  res = predictor.test();

  predictor.saveTopKScores();

  switch(res.problemType) {
    case binary:
    case multiclass:
      LOG_INFO("Accuracy: " + std::to_string(res.accuracy));
      break;
    case multilabel:
      LOG_INFO("Prec@1: " + std::to_string(res.precision1));
      LOG_INFO("Prec@3: " + std::to_string(res.precision3));
      LOG_INFO("Prec@5: " + std::to_string(res.precision5));
      break;
    default:
      assert(false);
  }

  printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

  std::ifstream infile("/spiffs/detailedPrediction", std::ios::in|std::ios::binary);
  assert(infile.is_open());

  std::string line;

  while(getline(infile, line)){
    std::cout << line << std::endl;
  }
  infile.close();
}
