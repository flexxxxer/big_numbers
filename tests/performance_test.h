#ifndef PERFORMANCE_TEST_H
#define PERFORMANCE_TEST_H

#include <ctime>
#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <numeric>
#include <iostream>
#include <string>

struct benchmark_info
{
	uint32_t calc_count;
	uint32_t preview_calc_count;

	uint32_t average_runtime_ms;
	uint32_t median_runtime_ms;
	uint32_t moda_runtime_ms;
	uint32_t standard_deviation_runtime_ms;

	uint32_t max_runtime_ms;
	uint32_t min_runtime_ms;
};

struct math_statistics
{
	static uint32_t average(const std::vector<uint32_t>& values)
	{
		uint64_t sum = 0;

		for (uint32_t value : values)
			sum += value;

		return static_cast<uint32_t>(sum / values.size());
	}

	static uint32_t median(std::vector<uint32_t> values)
	{
		std::sort(values.begin(), values.end());
		if (values.size() % 2 == 1)
		{
			const uint32_t index = static_cast<uint32_t>(values.size() / 2 + 1);
			return values[index];
		}

		const uint32_t index = static_cast<uint32_t>(values.size() / 2);
		return (values[index] + values[index - 1]) / 2;
	}

	static uint32_t moda(std::vector<uint32_t> values)
	{
		// std::sort(values.begin(), values.end());
		return most_common(values.begin(), values.end());
	}
	
	static uint32_t standard_deviation(std::vector<uint32_t> values)
	{
		const double sum = std::accumulate(values.begin(), values.end(), 0.0);
		double mean = sum / values.size();

		std::vector<double> diff(values.size());
		std::transform(values.begin(), values.end(), diff.begin(), [mean](double x) { return x - mean; });
		const double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		const double stdev = std::sqrt(sq_sum / values.size());

		return static_cast<uint32_t>(stdev);
	}

	template<class InputIt, class T = typename std::iterator_traits<InputIt>::value_type>
	static T most_common(InputIt begin, InputIt end)
	{
		std::map<T, int> counts;
		for (InputIt it = begin; it != end; ++it) {
			if (counts.find(*it) != counts.end()) {
				++counts[*it];
			}
			else {
				counts[*it] = 1;
			}
		}
		return std::max_element(counts.begin(), counts.end(),
			[](const std::pair<T, int>& pair1, const std::pair<T, int>& pair2) {
				return pair1.second < pair2.second; })->first;
	}
};

template <typename FunctionReturnResult, typename DataForPerform>
class performance_test
{
	DataForPerform data_;
	std::function<FunctionReturnResult(DataForPerform)> benchmarkable_function_;
	uint32_t calc_count_;
	uint32_t preview_calc_count_;

	std::string test_name_;
public:
	performance_test(std::function<FunctionReturnResult(DataForPerform)> benchmarkable_function, DataForPerform perform_data, 
		std::string name, uint32_t calc_count = 10, uint32_t preview_calc_count = 5)
		: benchmarkable_function_(std::move(benchmarkable_function)
		) {
		calc_count_ = calc_count;
		preview_calc_count_ = preview_calc_count;
		data_ = perform_data;
		test_name_ = name;
	}

	[[nodiscard]] benchmark_info perform() const
	{
		// perform warming up
		for (uint32_t i = 0; i < preview_calc_count_; i++)
			benchmarkable_function_(data_);

		std::vector<uint32_t> run_times(calc_count_);

		for(uint32_t i = 0; i < calc_count_; i++)
		{
			const uint32_t timer = clock();
			benchmarkable_function_(data_);
			const uint32_t run_time = clock() - timer;

			run_times[i] = run_time; // save result
		}
		
		benchmark_info info {};
		info.calc_count = this->calc_count_;
		info.preview_calc_count = this->preview_calc_count_;
		
		info.average_runtime_ms = math_statistics::average(run_times);
		info.median_runtime_ms = math_statistics::median(run_times);
		info.moda_runtime_ms = math_statistics::moda(run_times);
		info.standard_deviation_runtime_ms = math_statistics::standard_deviation(run_times);

		info.max_runtime_ms = *std::max_element(run_times.begin(), run_times.end());
		info.min_runtime_ms = *std::min_element(run_times.begin(), run_times.end());
		
		return info;
	}

	void print_performance_test_info_to_stream(std::iostream& stream, benchmark_info info) const
	{
		stream << this->test_name_ << " benchmark" << std::endl;
		stream << "max time: " << info.max_runtime_ms << "ms" << std::endl;
		stream << "min time: " << info.min_runtime_ms << "ms" << std::endl;
		stream << "average time: " << info.average_runtime_ms << "ms" << std::endl;
		stream << "median time: " << info.median_runtime_ms << "ms" << std::endl;
		stream << "moda time: " << info.moda_runtime_ms << "ms" << std::endl;
		stream << "standard deviation time: " << info.standard_deviation_runtime_ms << "ms" << std::endl;
	}
};

#endif