// Copyright: Wieger Wesselink 2022 - 2024
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file multilayer_perceptron_test.cpp
/// \brief Tests for multilayer perceptrons.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest/doctest.h"
#include "nerva/neural_networks/multilayer_perceptron.h"
#include "nerva/neural_networks/loss_functions.h"
#include "nerva/utilities/string_utility.h"
#include <iostream>

using namespace nerva;

inline
void check_equal_matrices(const std::string& name1, const eigen::matrix& X1, const std::string& name2, const eigen::matrix& X2, scalar epsilon = 1e-5)
{
  scalar error = (X2 - X1).squaredNorm();
  if (error > epsilon)
  {
    CHECK_LE(error, epsilon);
    print_cpp_matrix(name1, X1);
    print_cpp_matrix(name2, X2);
  }
}

// tag::doc[]
void construct_mlp(multilayer_perceptron& M,
                   const eigen::matrix& W1,
                   const eigen::matrix& b1,
                   const eigen::matrix& W2,
                   const eigen::matrix& b2,
                   const eigen::matrix& W3,
                   const eigen::matrix& b3,
                   const std::vector<long>& sizes,
                   long N
                  )
{
  long batch_size = N;

  auto layer1 = std::make_shared<relu_layer<eigen::matrix>>(sizes[0], sizes[1], batch_size);
  M.layers.push_back(layer1);
  auto optimizer_W1 = std::make_shared<gradient_descent_optimizer<eigen::matrix>>(layer1->W, layer1->DW);
  auto optimizer_b1 = std::make_shared<gradient_descent_optimizer<eigen::matrix>>(layer1->b, layer1->Db);
  layer1->optimizer = make_composite_optimizer(optimizer_W1, optimizer_b1);
  layer1->W = W1;
  layer1->b = b1;

  auto layer2 = std::make_shared<relu_layer<eigen::matrix>>(sizes[1], sizes[2], batch_size);
  M.layers.push_back(layer2);
  auto optimizer_W2 = std::make_shared<gradient_descent_optimizer<eigen::matrix>>(layer2->W, layer2->DW);
  auto optimizer_b2 = std::make_shared<gradient_descent_optimizer<eigen::matrix>>(layer2->b, layer2->Db);
  layer2->optimizer = make_composite_optimizer(optimizer_W2, optimizer_b2);
  layer2->W = W2;
  layer2->b = b2;

  auto layer3 = std::make_shared<linear_layer<eigen::matrix>>(sizes[2], sizes[3], batch_size);
  M.layers.push_back(layer3);
  auto optimizer_W3 = std::make_shared<gradient_descent_optimizer<eigen::matrix>>(layer3->W, layer3->DW);
  auto optimizer_b3 = std::make_shared<gradient_descent_optimizer<eigen::matrix>>(layer3->b, layer3->Db);
  layer3->optimizer = make_composite_optimizer(optimizer_W3, optimizer_b3);
  layer3->W = W3;
  layer3->b = b3;
}
// end::doc[]

void test_mlp_execution(const eigen::matrix& X,
                        const eigen::matrix& T,
                        const eigen::matrix& W1,
                        const eigen::matrix& b1,
                        const eigen::matrix& W2,
                        const eigen::matrix& b2,
                        const eigen::matrix& W3,
                        const eigen::matrix& b3,
                        const eigen::matrix& Y1,
                        const eigen::matrix& DY1,
                        const eigen::matrix& Y2,
                        const eigen::matrix& DY2,
                        scalar lr,
                        const std::vector<long>& sizes,
                        long N
                       )
{
  multilayer_perceptron M;
  long K = sizes.back(); // the output size of the MLP
  construct_mlp(M, W1, b1, W2, b2, W3, b3, sizes, N);
  // M.info("M");

  eigen::matrix Y(N, K);
  eigen::matrix DY(N, K);

  softmax_cross_entropy_loss loss;

  M.feedforward(X, Y);
  DY = loss.gradient(Y, T) / N; // take the average of the gradients in the batch

  check_equal_matrices("Y", Y, "Y1", Y1);
  check_equal_matrices("DY", DY, "DY1", DY1);

  M.backpropagate(Y, DY);
  M.optimize(lr);
  M.feedforward(X, Y);
  M.backpropagate(Y, DY);

  check_equal_matrices("Y", Y, "Y2", Y2);
  check_equal_matrices("DY", DY, "DY2", DY2);
}

//--- begin generated code ---//
TEST_CASE("test_mlp1")
{
  eigen::matrix X {
    {0.37454012, 0.95071429},
    {0.73199391, 0.59865850},
    {0.15601864, 0.15599452},
    {0.05808361, 0.86617613},
    {0.60111499, 0.70807260},
  };

  eigen::matrix T {
    {0.00000000, 1.00000000, 0.00000000},
    {1.00000000, 0.00000000, 0.00000000},
    {0.00000000, 1.00000000, 0.00000000},
    {0.00000000, 1.00000000, 0.00000000},
    {0.00000000, 1.00000000, 0.00000000},
  };

  eigen::matrix W1 {
    {0.60928798, 0.12283446},
    {-0.40647557, 0.16275182},
    {-0.51447225, 0.31192824},
    {-0.52783161, -0.25182083},
    {0.17900684, 0.27646673},
    {0.04572495, 0.12897399},
  };

  eigen::matrix b1 {
    {0.38675582, 0.42659640, 0.37051007, -0.11596697, 0.48523235, 0.04033934},
  };

  eigen::matrix W2 {
    {0.38505706, -0.13348164, 0.31668609, -0.32666627, -0.27552345, -0.12539147},
    {0.28924564, 0.27090782, 0.15212868, 0.15059654, 0.08706193, 0.06441139},
    {-0.33077568, 0.11333510, -0.19510441, 0.09439045, 0.31387877, 0.26053587},
    {-0.38992766, -0.38738862, -0.18559195, 0.10539542, 0.01223989, -0.13228671},
  };

  eigen::matrix b2 {
    {0.28535178, 0.29878062, 0.18463619, 0.25079638},
  };

  eigen::matrix W3 {
    {0.00587273, -0.32241952, 0.30951124, 0.33368146},
    {-0.30861658, 0.04479754, -0.10331273, -0.24567801},
    {-0.02842641, 0.25743872, -0.01068246, 0.43675703},
  };

  eigen::matrix b3 {
    {-0.38727069, 0.00397784, 0.22307467},
  };

  eigen::matrix Y1 {
    {-0.57443899, -0.10925630, 0.41034985},
    {-0.56981552, -0.11173054, 0.39652818},
    {-0.53720498, -0.10108448, 0.37866449},
    {-0.56281579, -0.10445327, 0.40684068},
    {-0.57073528, -0.11069612, 0.40081441},
  };

  eigen::matrix DY1 {
    {0.03795389, -0.13956583, 0.10161193},
    {-0.16160758, 0.06070010, 0.10090748},
    {0.03963816, -0.13869186, 0.09905367},
    {0.03832504, -0.13938963, 0.10106459},
    {0.03826893, -0.13937680, 0.10110789},
  };

  eigen::matrix Y2 {
    {-0.57339072, -0.09924223, 0.40067863},
    {-0.56873512, -0.10201052, 0.38706493},
    {-0.53640091, -0.09185813, 0.36971307},
    {-0.56188405, -0.09455847, 0.39731762},
    {-0.56967103, -0.10088711, 0.39129135},
  };

  eigen::matrix DY2 {
    {0.03805648, -0.13885672, 0.10080025},
    {-0.16150525, 0.06138998, 0.10011526},
    {0.03972618, -0.13803604, 0.09830986},
    {0.03842213, -0.13868901, 0.10026687},
    {0.03837107, -0.13868113, 0.10031006},
  };

  scalar lr = 0.01;
  std::vector<long> sizes = {2, 6, 4, 3};
  long N = 5;
  test_mlp_execution(X, T, W1, b1, W2, b2, W3, b3, Y1, DY1, Y2, DY2, lr, sizes, N);
}

TEST_CASE("test_mlp2")
{
  eigen::matrix X {
    {0.00077877, 0.99221158, 0.61748153},
    {0.61165315, 0.00706631, 0.02306242},
    {0.52477467, 0.39986098, 0.04666566},
    {0.97375554, 0.23277134, 0.09060644},
  };

  eigen::matrix T {
    {0.00000000, 1.00000000, 0.00000000, 0.00000000},
    {0.00000000, 0.00000000, 1.00000000, 0.00000000},
    {0.00000000, 0.00000000, 0.00000000, 1.00000000},
    {0.00000000, 0.00000000, 1.00000000, 0.00000000},
  };

  eigen::matrix W1 {
    {-0.11894515, -0.21960714, 0.43446422},
    {-0.02698070, -0.45792788, -0.37403610},
    {0.39320531, 0.28537872, -0.01389291},
    {-0.45905599, -0.14313057, 0.33424249},
    {-0.06112428, 0.56518370, -0.17656146},
  };

  eigen::matrix b1 {
    {-0.53593326, 0.20641872, 0.37182173, 0.22043863, -0.46697491},
  };

  eigen::matrix W2 {
    {0.29050902, -0.00753555, -0.32190365, 0.32120478, -0.18679278},
    {-0.39732596, 0.24707054, -0.15472746, -0.41491807, -0.30236724},
  };

  eigen::matrix b2 {
    {0.33665219, 0.28667694},
  };

  eigen::matrix W3 {
    {-0.33008054, -0.57166249},
    {0.08401687, 0.16038688},
    {0.40199116, 0.28578538},
    {0.52477258, -0.28745869},
  };

  eigen::matrix b3 {
    {0.28106654, 0.31312311, 0.19998005, -0.37107974},
  };

  eigen::matrix Y1 {
    {0.16929087, 0.34259441, 0.30795681, -0.27542847},
    {0.10091013, 0.36248556, 0.32266474, -0.36658585},
    {0.14072615, 0.35151780, 0.29715776, -0.36289388},
    {0.16019323, 0.34641331, 0.27757174, -0.38182792},
  };

  eigen::matrix DY1 {
    {0.06283031, -0.17528050, 0.07217571, 0.04027449},
    {0.05989732, 0.07780512, -0.17523226, 0.03752982},
    {0.06237020, 0.07700568, 0.07293139, -0.21230726},
    {0.06392547, 0.07701021, -0.17811292, 0.03717724},
  };

  eigen::matrix Y2 {
    {0.16396461, 0.34277371, 0.31224406, -0.27354622},
    {0.09554848, 0.36259505, 0.32708561, -0.36450708},
    {0.13541058, 0.35163751, 0.30156288, -0.36074781},
    {0.15458742, 0.34661475, 0.28218639, -0.37964958},
  };

  eigen::matrix DY2 {
    {0.06248016, -0.17528671, 0.07246679, 0.04033978},
    {0.05955379, 0.07778328, -0.17493030, 0.03759324},
    {0.06201931, 0.07698978, 0.07322948, -0.21223859},
    {0.06354988, 0.07700363, -0.17780113, 0.03724762},
  };

  scalar lr = 0.01;
  std::vector<long> sizes = {3, 5, 2, 4};
  long N = 4;
  test_mlp_execution(X, T, W1, b1, W2, b2, W3, b3, Y1, DY1, Y2, DY2, lr, sizes, N);
}

TEST_CASE("test_mlp3")
{
  eigen::matrix X {
    {0.98323089, 0.46676290, 0.85994041, 0.68030757, 0.45049927, 0.01326496},
    {0.94220173, 0.56328821, 0.38541651, 0.01596625, 0.23089382, 0.24102546},
    {0.68326354, 0.60999668, 0.83319491, 0.17336465, 0.39106062, 0.18223609},
    {0.75536144, 0.42515588, 0.20794167, 0.56770033, 0.03131329, 0.84228480},
    {0.44975412, 0.39515024, 0.92665887, 0.72727197, 0.32654077, 0.57044399},
    {0.52083427, 0.96117204, 0.84453386, 0.74732012, 0.53969210, 0.58675116},
    {0.96525532, 0.60703427, 0.27599919, 0.29627350, 0.16526695, 0.01563641},
    {0.42340147, 0.39488152, 0.29348817, 0.01407982, 0.19884241, 0.71134198},
  };

  eigen::matrix T {
    {0.00000000, 0.00000000, 1.00000000},
    {1.00000000, 0.00000000, 0.00000000},
    {0.00000000, 0.00000000, 1.00000000},
    {1.00000000, 0.00000000, 0.00000000},
    {0.00000000, 1.00000000, 0.00000000},
    {0.00000000, 0.00000000, 1.00000000},
    {0.00000000, 1.00000000, 0.00000000},
    {1.00000000, 0.00000000, 0.00000000},
  };

  eigen::matrix W1 {
    {0.19299567, 0.01329524, -0.17483680, 0.24752843, 0.05703489, 0.33503324},
    {-0.11934533, 0.11972965, -0.12702829, 0.29243055, 0.04991569, -0.16361657},
  };

  eigen::matrix b1 {
    {-0.32831228, -0.03975018},
  };

  eigen::matrix W2 {
    {0.53117746, -0.03685540},
    {0.55761832, -0.43632591},
  };

  eigen::matrix b2 {
    {-0.61475408, -0.00805780},
  };

  eigen::matrix W3 {
    {0.48559618, -0.36571199},
    {-0.62847406, -0.08527689},
    {0.27863246, -0.11907403},
  };

  eigen::matrix b3 {
    {0.56641662, -0.53879106, -0.35676616},
  };

  eigen::matrix Y1 {
    {0.56641662, -0.53879106, -0.35676616},
    {0.56641662, -0.53879106, -0.35676616},
    {0.56641662, -0.53879106, -0.35676616},
    {0.52628022, -0.54815006, -0.36983439},
    {0.56641662, -0.53879106, -0.35676616},
    {0.56641662, -0.53879106, -0.35676616},
    {0.56641662, -0.53879106, -0.35676616},
    {0.56641662, -0.53879106, -0.35676616},
  };

  eigen::matrix DY1 {
    {0.07232144, 0.02394868, -0.09627011},
    {-0.05267856, 0.02394868, 0.02872988},
    {0.07232144, 0.02394868, -0.09627011},
    {-0.05355693, 0.02439726, 0.02915967},
    {0.07232144, -0.10105132, 0.02872988},
    {0.07232144, 0.02394868, -0.09627011},
    {0.07232144, -0.10105132, 0.02872988},
    {-0.05267856, 0.02394868, 0.02872988},
  };

  eigen::matrix Y2 {
    {0.56438971, -0.53821146, -0.35531884},
    {0.56438971, -0.53821146, -0.35531884},
    {0.56438971, -0.53821146, -0.35531884},
    {0.52435845, -0.54755038, -0.36835843},
    {0.56438971, -0.53821146, -0.35531884},
    {0.56438971, -0.53821146, -0.35531884},
    {0.56438971, -0.53821146, -0.35531884},
    {0.56438971, -0.53821146, -0.35531884},
  };

  eigen::matrix DY2 {
    {0.07222755, 0.02398001, -0.09620755},
    {-0.05277245, 0.02398001, 0.02879245},
    {0.07222755, 0.02398001, -0.09620755},
    {-0.05364873, 0.02442742, 0.02922131},
    {0.07222755, -0.10101999, 0.02879245},
    {0.07222755, 0.02398001, -0.09620755},
    {0.07222755, -0.10101999, 0.02879245},
    {-0.05277245, 0.02398001, 0.02879245},
  };

  scalar lr = 0.01;
  std::vector<long> sizes = {6, 2, 2, 3};
  long N = 8;
  test_mlp_execution(X, T, W1, b1, W2, b2, W3, b3, Y1, DY1, Y2, DY2, lr, sizes, N);
}


//--- end generated code ---//
