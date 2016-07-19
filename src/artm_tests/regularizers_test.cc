// Copyright 2014, Additive Regularization of Topic Models.

#include <memory>

#include "gtest/gtest.h"

#include "artm/cpp_interface.h"
#include "artm/core/common.h"
#include "artm/core/instance.h"

#include "artm_tests/test_mother.h"
#include "artm_tests/api.h"

// artm_tests.exe --gtest_filter=Regularizers.TopicSelection
TEST(Regularizers, TopicSelection) {
  int nTopics = 10;

  // create master
  ::artm::MasterModelConfig master_config = ::artm::test::TestMother::GenerateMasterModelConfig(nTopics);
  master_config.set_cache_theta(true);

  // create regularizer
  ::artm::RegularizerConfig* regularizer_config = master_config.add_regularizer_config();

  regularizer_config->set_name("HierarchySparsingRegularizer");
  regularizer_config->set_type(::artm::RegularizerType_HierarchySparsingTheta);
  regularizer_config->set_tau(1000.0f);

  ::artm::HierarchySparsingThetaConfig internal_config;
  for (int i = 0; i < nTopics; ++i)
    internal_config.add_topic_proportion(static_cast<float>(i) / nTopics);

  regularizer_config->set_config(internal_config.SerializeAsString());
  artm::MasterModel master(master_config);
  ::artm::test::Api api(master);

  // iterations
  auto batches = ::artm::test::TestMother::GenerateBatches(1, 5);
  (*batches[0]).set_description("__parent_phi_matrix_batch__");
  auto offline_args = api.Initialize(batches);
  for (int iter = 0; iter < 3; ++iter)
    master.FitOfflineModel(offline_args);

  // get and check theta
  artm::GetThetaMatrixArgs args;
  ::artm::ThetaMatrix theta_matrix = master.GetThetaMatrix();

   //Uncomment to dump actual results
  for (int i = 0; i <= 9; ++i)
    std::cout << theta_matrix.item_weights(0).value(i) << std::endl;
  float expected_values[] = { 0.41836f, 0.262486f, 0.160616f, 0.0845677f, 0.032849f,
                              0.022987f, 0.0103793f, 0.0040327f, 0.00267936f, 0.00104289f };
  //for (int i = 0; i < nTopics; ++i)
  //  ASSERT_NEAR(theta_matrix.item_weights(0).value(i), expected_values[i], 0.00001);

}
