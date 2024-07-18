#include "math.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <vector>

template<class function>
void test(const std::string& label, function func, const std::string& expected) {
	std::ostringstream stream;
	auto buffer{std::cout.rdbuf()};
	auto flags{std::cout.flags()};

	std::cout.rdbuf(stream.rdbuf());
	func();
	std::cout.rdbuf(buffer);
	std::cout.flags(flags);
	std::string actual{stream.str()};

	auto p1{std::ranges::find_if_not(actual, std::isspace)}, e1{actual.end()};
	auto p2{std::ranges::find_if_not(expected, std::isspace)}, e2{expected.end()};

	do {
		auto n1{std::ranges::find_if_not(p1, e1, std::isspace)};
		auto n2{std::ranges::find_if_not(p2, e2, std::isspace)};
		if ((p1 == n1) != (p2 == n2)) {
			break;
		}
		p1 = n1;
		p2 = n2;
	} while (p1 != e1 && p2 != e2 && *p1++ == *p2++);
	p1 = std::ranges::find_if_not(p1, e1, std::isspace);
	p2 = std::ranges::find_if_not(p2, e2, std::isspace);

	static constexpr auto reset{"\033[m"}, red{"\033[31m"}, green{"\033[32m"}, yellow{"\033[33m"};
	std::size_t space{std::max(label.size() + 1, std::size_t{36})};

	std::cout << std::left;
	if (p1 == e1 && p2 == e2) {
		std::cout << green << std::setw(space) << label << "Test Passed\n" << reset;
	}
	else {
		std::cout << red << std::setw(space) << label << "Test Failed\n"
			<< yellow << "[Expected Output]\n" << reset << expected << '\n'
			<< yellow << "[Actual Output]\n" << reset << actual << '\n';
	}
	std::cout.flags(flags);
}

namespace math_test {

	void sqrt() {
		std::vector<std::uint64_t> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1182280556, 1078161935, 1727740828, 1988114385, 1915211379};
		for (auto& i : a) {
			std::cout << concrete::square_root(i) << ' ';
		}
	}

	void pow() {
		std::vector<std::uint64_t> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 3002069754, 3272062159, 2873182719, 2496520859, 3004049190};
		for (auto& i : a) {
			std::cout << concrete::power(i, i) << ' ';
		}
	}

	void gcd_lcm() {
		for (int i{0}; i < 10; ++i) {
			for (int j{0}; j < 10; ++j) {
				std::cout << '{'
					<< concrete::greatest_common_divisor(i, j) << ','
					<< concrete::least_common_multiple(i, j)
					<< '}' << ' ';
			}
			std::cout << '\n';
		}
	}

	void kronecker_symbol() {
		for (int i{0}; i < 10; ++i) {
			for (int j{0}; j < 10; ++j) {
				std::cout << concrete::kronecker_symbol(i, j) << ' ';
			}
			std::cout << '\n';
		}
	}

	void is_prime() {
		std::vector<std::uint64_t> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 4204940309, 4080787278, 3932737873, 2564387963, 2207258474};
		for (auto& i : a) {
			std::cout << concrete::is_prime(i) << ' ';
		}
	}

	void run() {
		test("sqrt", sqrt, "0 1 1 1 2 2 2 2 2 3 34384 32835 41566 44588 43763");
		test("pow", pow, "1 1 4 27 256 3125 46656 823543 16777216 387420489 0 1783393135476470575 15480720596823619071 16371189851142813251 0");
		test("gcd/lcm", gcd_lcm, R"(
{0,0}	{1,0}	{2,0}	{3,0}	{4,0}	{5,0}	{6,0}	{7,0}	{8,0}	{9,0}
{1,0}	{1,1}	{1,2}	{1,3}	{1,4}	{1,5}	{1,6}	{1,7}	{1,8}	{1,9}
{2,0}	{1,2}	{2,2}	{1,6}	{2,4}	{1,10}	{2,6}	{1,14}	{2,8}	{1,18}
{3,0}	{1,3}	{1,6}	{3,3}	{1,12}	{1,15}	{3,6}	{1,21}	{1,24}	{3,9}
{4,0}	{1,4}	{2,4}	{1,12}	{4,4}	{1,20}	{2,12}	{1,28}	{4,8}	{1,36}
{5,0}	{1,5}	{1,10}	{1,15}	{1,20}	{5,5}	{1,30}	{1,35}	{1,40}	{1,45}
{6,0}	{1,6}	{2,6}	{3,6}	{2,12}	{1,30}	{6,6}	{1,42}	{2,24}	{3,18}
{7,0}	{1,7}	{1,14}	{1,21}	{1,28}	{1,35}	{1,42}	{7,7}	{1,56}	{1,63}
{8,0}	{1,8}	{2,8}	{1,24}	{4,8}	{1,40}	{2,24}	{1,56}	{8,8}	{1,72}
{9,0}	{1,9}	{1,18}	{3,9}	{1,36}	{1,45}	{3,18}	{1,63}	{1,72}	{9,9}
)");
		test("kronecker_symbol", kronecker_symbol, R"(
0	1	0	0	0	0	0	0	0	0
1	1	1	1	1	1	1	1	1	1
0	1	0	-1	0	-1	0	1	0	1
0	1	-1	0	1	-1	0	-1	-1	0
0	1	0	1	0	1	0	1	0	1
0	1	-1	-1	1	0	1	-1	-1	1
0	1	0	0	0	1	0	-1	0	0
0	1	1	1	1	-1	1	0	1	1
0	1	0	-1	0	-1	0	1	0	1
0	1	1	0	1	1	0	1	1	0
)");
		test("is_prime", is_prime, "0 0 1 1 0 1 0 1 0 0 1 0 1 1 0");
	}

}

//int main() {
//	math_test::run();
//}