#ifndef GALTON_GALTON_H_
#define GALTON_GALTON_H_

#include <cstdint>
#include <random>
#include <unordered_set>
#include <vector>
#include <iostream>

class GaltonBoard {
 public:
  // Constructor for beta scheme
  explicit GaltonBoard(uint64_t bins, uint64_t balls, double beta, uint16_t k_choices)
      : number_of_bins_(bins),
        balls_(balls),
        bins_(bins, 0),
        beta_value_(beta),
        k_choices_(k_choices),
        rng_(std::random_device{}()) {}

  // Constructor for d-choice scheme
  explicit GaltonBoard(uint64_t bins, uint64_t balls, uint16_t d_choice, uint16_t k_choices)
      : number_of_bins_(bins),
        balls_(balls),
        bins_(bins, 0),
        d_choice_(d_choice),
        k_choices_(k_choices),
        rng_(std::random_device{}()) {}

  void Run(const uint64_t);
  double CalculateMaxGap(double load_factor) const;

 private:
  void SimulateBatch(const uint64_t);

  uint64_t ChooseScheme();

  uint64_t ChooseBin(const std::unordered_set<uint64_t>&);
  uint64_t ChooseBinUncertainty(const std::unordered_set<uint64_t>&);

  bool FilterAndReturn(std::vector<uint64_t>&, uint64_t);
  
  uint64_t SelectRandom(const std::vector<uint64_t>&);

  
  const uint64_t number_of_bins_;
  const uint64_t balls_;
  std::vector<uint64_t> bins_;  // counters

  const double beta_value_ = -1;  // default beta value
  const uint16_t d_choice_ = 0;   // default d-choice
  const uint16_t k_choices_ = 0;

  std::mt19937 rng_;
};

#endif  // GALTON_H