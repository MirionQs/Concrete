#define BENCHMARK_STATIC_DEFINE
#pragma comment(lib, "shlwapi.lib")
#include <benchmark/benchmark.h>

#include <random>
#include <bit>

constexpr uint64_t RL(uint64_t x, uint64_t y) noexcept {
	uint64_t res{1};
	while (y != 0) {
		if (y & 1) {
			res *= x;
		}
		x *= x;
		y >>= 1;
	}
	return res;
}

constexpr uint64_t RL4(uint64_t x, uint64_t y) noexcept {
	uint64_t res{1};
	while (y != 0) {
		uint64_t x2{x * x}, x3{x2 * x};
		switch (y & 3) {
		case 1:
			res *= x;
			break;
		case 2:
			res *= x2;
			break;
		case 3:
			res *= x3;
			break;
		}
		x *= x3;
		y >>= 2;
	}
	return res;
}

constexpr uint64_t LR(uint64_t x, uint64_t y) noexcept {
	uint64_t res{1};
	unsigned n{(unsigned)std::countl_zero(y)};
	y <<= n;
	while (n != 64) {
		res *= res;
		if (y >> 63) {
			res *= x;
		}
		++n;
		y <<= 1;
	}
	return res;
}

constexpr uint64_t LR4(uint64_t x, uint64_t y) noexcept {
	uint64_t res{1}, tab[4]{1, x, x * x, x * x * x};
	unsigned n{(unsigned)std::countl_zero(y) & ~1};
	y <<= n;
	while (n != 64) {
		res *= res;
		res *= res;
		res *= tab[y >> 62];
		n += 2;
		y <<= 2;
	}
	return res;
}

constexpr uint64_t LR16(uint64_t x, uint64_t y) noexcept {
	uint64_t res{1}, tab[16]{1};
	for (size_t i{1}; i != 16; ++i) {
		tab[i] = tab[i - 1] * x;
	}
	unsigned n{(unsigned)std::countl_zero(y) & ~3};
	y <<= n;
	while (n != 64) {
		res *= res;
		res *= res;
		res *= res;
		res *= res;
		res *= tab[y >> 60];
		n += 4;
		y <<= 4;
	}
	return res;
}

constexpr bool test{LR16(3272062159, 3272062159) == 1783393135476470575};

#define BMS(func, scope) \
static void func##_##scope(benchmark::State& state) { \
	std::mt19937_64 eng{std::random_device{}()}; \
	std::uniform_int_distribution dist; \
	for (auto _ : state) { \
		auto res{scope::func(dist(eng), dist(eng))}; \
		benchmark::DoNotOptimize(res); \
	} \
} \
BENCHMARK(func##_##scope)

#define BM(func) BMS(func,)

BMS(pow, std);
BM(RL);
BM(RL4);
BM(LR);
BM(LR4);
BM(LR16);

//BENCHMARK_MAIN();