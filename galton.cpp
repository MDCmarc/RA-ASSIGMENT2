#include "galton.h"
#include <algorithm>

uint16_t partition(std::vector<uint64_t>& v, uint16_t low, uint16_t high) {
    uint64_t pivot = v[high];
    uint16_t i = low;
    for (uint16_t j = low; j < high; ++j) {
        if (v[j] < pivot) {
            std::swap(v[i], v[j]);
            i++;
        }
    }
    std::swap(v[i], v[high]);
    return i;
}
uint64_t quickSelect(std::vector<uint64_t>& v, uint16_t i, uint16_t j, uint16_t k) {
  if(i == j) return v[i];

  uint16_t q = partition(v, i, j);
  if (q == k) return v[q];
  if (k < q) return quickSelect(v, i, q - 1, k);
  return quickSelect(v, q + 1, j, k);
}

uint64_t GaltonBoard::SelectRandom(const std::vector<uint64_t>& v) {
  std::uniform_int_distribution<uint64_t> dist(0, v.size() - 1);
  return v[dist(rng_)];
}

bool GaltonBoard::FilterAndReturn(std::vector<uint64_t>& bins_vector, uint64_t filter_value) {
  std::vector<uint64_t> below_filter;
  for (auto bin : bins_vector) {
    if (bins_[bin] < filter_value) below_filter.push_back(bin);
  }

  if (below_filter.empty()) return false;

  bins_vector = std::move(below_filter);
  return true;
}

uint64_t GaltonBoard::ChooseBinUncertainty(const std::unordered_set<uint64_t>& chosen_bins) {
  std::vector<uint64_t> candidate_bins(chosen_bins.begin(), chosen_bins.end());
  
  // Calculate percentiles
  std::vector<uint64_t> all_loads = bins_;
  size_t total_bins = all_loads.size();
  uint64_t median_load = quickSelect(all_loads, 0, total_bins - 1, total_bins / 2);
  uint64_t q1_load = quickSelect(all_loads, 0, total_bins - 1, total_bins / 4);
  uint64_t q3_load = quickSelect(all_loads, 0, total_bins - 1, (3 * total_bins) / 4);
  
  // First filter based on median load
  bool is_below_median = FilterAndReturn(candidate_bins, median_load);
  if(is_below_median && candidate_bins.size() == 1) return candidate_bins[0];

  if (k_choices_ == 1) return SelectRandom(candidate_bins);

  // We ask the second question 
  if (is_below_median) (void)FilterAndReturn(candidate_bins,q1_load);
  else (void)FilterAndReturn(candidate_bins,q3_load);
  
  return SelectRandom(candidate_bins);
}

uint64_t GaltonBoard::ChooseBin(const std::unordered_set<uint64_t>& chosen_bins)  {
  uint64_t min_ball_count = std::numeric_limits<uint64_t>::max();
  std::vector<uint64_t> min_bins;

  for (auto bin : chosen_bins) {
    const uint64_t count = bins_[bin];
    if (count < min_ball_count) {
      min_ball_count = count;
      min_bins = {bin};
    } else if (count == min_ball_count) {
      min_bins.push_back(bin);
    }
  }

  return SelectRandom(min_bins);
}

uint64_t GaltonBoard::ChooseScheme() {
  if (beta_value_ == -1) return d_choice_;
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  return (dist(rng_) < beta_value_) ? 1 : 2;
}

void GaltonBoard::SimulateBatch(const uint64_t batch_size) {
  
  uint64_t batch_counts[batch_size];

  for(uint64_t i = 0; i < batch_size; ++i){
    std::uniform_int_distribution<uint64_t> dist(0, bins_.size() - 1);
    const uint16_t number_possible_bins = ChooseScheme();

    std::unordered_set<uint64_t> chosen_bins;
    chosen_bins.reserve(number_possible_bins);
    while (chosen_bins.size() < number_possible_bins) {
      chosen_bins.insert(dist(rng_));
    }

    batch_counts[i] = (k_choices_ == 0 ) ? ChooseBin(chosen_bins) : ChooseBinUncertainty(chosen_bins);
  }
  // Update bins_
  for (const auto chosen_bin : batch_counts) ++bins_[chosen_bin];
}

void GaltonBoard::Run(const uint64_t batch_size = 1) {
  uint64_t full_batches = balls_ / batch_size;
  uint64_t remaining_balls = balls_ % batch_size;
  
  // Run full batches
  for (uint64_t b = 0; b < full_batches; ++b) {
      SimulateBatch(batch_size);
  }

  // Run remaining balls
  if (remaining_balls > 0) {
      SimulateBatch(remaining_balls);
  }
}

double GaltonBoard::CalculateMaxGap(double load_factor) const {
  double max_gap = 0;
  for (auto bin_count : bins_) {
    const double gap = bin_count - load_factor;
    if (gap > max_gap) max_gap = gap;
  }
  return max_gap;
}