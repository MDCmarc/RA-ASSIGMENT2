#include <getopt.h>  // Include for getopt_long
#include <omp.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "galton.h"

constexpr uint64_t TRIALS = 100;
constexpr uint64_t NUMBER_BINS = 25;
constexpr uint64_t MAX_BALLS = NUMBER_BINS * NUMBER_BINS;


void Save(const std::vector<double>& maxLoads, 
          const char scheme,  const uint64_t D_CHOICE, const double BETA,
          const char exp, const uint16_t exp_value) {
  char filename[256];
  if (BETA != -1){
    int value = BETA * 10;
    std::sprintf(filename, "Simulations/%c%d_%c%u.csv", scheme, value, exp, exp_value);
  }
  else std::sprintf(filename, "Simulations/%c%lu_%c%u.csv", scheme, D_CHOICE, exp, exp_value);

  std::ofstream file(filename);

  file << "balls,averageMaxLoad\n";
  for (size_t i = 0; i < maxLoads.size(); ++i) {
    file << i << ", " << maxLoads[i] << "\n";
  }
}


double RunAllTrials(const uint64_t number_of_balls, const uint16_t D_CHOICE, const double BETA,
                    const uint16_t K_CHOICE, const uint64_t BATCH_SIZE) {
  const double load_factor = static_cast<double>(number_of_balls) / static_cast<double>(NUMBER_BINS);
  double gap_sum = 0.0;


#pragma omp parallel for reduction(+ : gap_sum)
  for (uint64_t t = 0; t < TRIALS; ++t) {

    if (BETA == -1) {
      GaltonBoard board = GaltonBoard(NUMBER_BINS, number_of_balls, D_CHOICE, K_CHOICE);
      board.Run(BATCH_SIZE);
      gap_sum += board.CalculateMaxGap(load_factor);
    } else {
      GaltonBoard board = GaltonBoard(NUMBER_BINS, number_of_balls, BETA, K_CHOICE);
      board.Run(BATCH_SIZE);
      gap_sum += board.CalculateMaxGap(load_factor);
    }
  }

  return gap_sum / TRIALS;
}


int main(int argc, char* argv[]) {
  char allocation_scheme = '0';  // 'D' or 'B'
  char experiment_type = '0';    // 'S' or 'K'

  uint16_t D_CHOICE = 0;
  double BETA = -1;
  uint16_t K_CHOICE = 0;
  uint64_t BATCH_SIZE = 1;

  // Define the long options for getopt
  struct option long_options[] = {
      {"D", required_argument, 0, 'D'},
      {"B", required_argument, 0, 'B'},
      {"S", required_argument, 0, 'S'},
      {"K", required_argument, 0, 'K'},
      {0, 0, 0, 0}  // End of options
  };

  // Parse command-line arguments
  int opt;
  while ((opt = getopt_long(argc, argv, "D:B:S:K:", long_options, nullptr)) !=-1) {
    switch (opt) {
      case 'D':
      case 'B':
        // Ensure only one allocation scheme is specified
        if (allocation_scheme != '0') {
          std::cerr << "Error: You can only specify one allocation scheme (-D or -B)" << std::endl;
          return 1;
        }
        allocation_scheme = opt;
        if (allocation_scheme == 'D') D_CHOICE = std::stoi(optarg);
        else BETA = std::stod(optarg);
        break;

      case 'S':
      case 'K':
        // Ensure only one experiment type is specified
        if (experiment_type != '0') {
          std::cerr << "Error: You can only specify one experiment type (-S or -K)" << std::endl;
          return 1;
        }
        experiment_type = opt;
        if (experiment_type == 'S') BATCH_SIZE = std::stoull(optarg);
        else K_CHOICE = std::stoi(optarg);
        break;

      case '?':
        std::cerr << "Error: Unknown option or missing argument" << std::endl;
        return 1;
      default:
        std::cerr << "Error: Invalid option" << std::endl;
        return 1;
    }
  }

  // Check for missing options
  if (allocation_scheme == '0') {
    std::cerr << "Error: An allocation scheme (-D or -B) must be specified." << std::endl;
    return 1;
  }
  std::cout << "Parsed options:" << std::endl;
  std::cout << "Allocation scheme: " << (allocation_scheme == 'D' ? "D" : "B") <<", Value: "<<(allocation_scheme == 'D' ? D_CHOICE : BETA) <<std::endl;
  std::cout << "Experiment type: " << (experiment_type == 'S' ? "S" : "K")<<", Value: " << (experiment_type == 'S' ? BATCH_SIZE : K_CHOICE) << std::endl;

  std::vector<double> maxLoads(MAX_BALLS + 1, 0.0);

#pragma omp parallel for
  for (uint64_t n = 0; n <= MAX_BALLS; ++n) {
    maxLoads[n] = RunAllTrials(n, D_CHOICE, BETA, K_CHOICE, BATCH_SIZE);
  }
  Save(maxLoads,
    allocation_scheme, D_CHOICE, BETA , 
    experiment_type, (experiment_type == 'S' ? BATCH_SIZE : K_CHOICE) );
  return 0;
}
