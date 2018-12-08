#pragma once

#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

#include <glm/glm.hpp>

std::string getStringFromFile(const std::string& file);

template <class ForwardIt>
void mergeSort(ForwardIt first, ForwardIt last)
{
	if (std::distance(first, last) <= 1) return;

	ForwardIt middle = std::next(first, std::distance(first, last) / 2);

	mergeSort(first, middle);
	mergeSort(middle, last);

	std::inplace_merge(first, middle, last);
}

template <class ForwardIt>
void quickSort(ForwardIt first, ForwardIt last)
{
	if (first == last) return;

	auto pivot = *std::next(first, std::distance(first, last) / 2);

	ForwardIt middle1 = std::partition(first, last, [pivot](const auto& elem) -> bool {return elem < pivot;});
	ForwardIt middle2 = std::partition(middle1, last, [pivot](const auto& elem) -> bool {return !(pivot < elem);});

	quickSort(first, middle1);
	quickSort(middle2, last);
}

template <class ForwardIt>
void quickSortStable(ForwardIt first, ForwardIt last)
{
	if (first == last) return;

	auto pivot = *std::next(first, std::distance(first, last) / 2);

	ForwardIt middle1 = std::stable_partition(first, last, [pivot](const auto& elem) -> bool {return elem < pivot;});
	ForwardIt middle2 = std::stable_partition(middle1, last, [pivot](const auto& elem) -> bool {return !(pivot < elem);});

	quickSortStable(first, middle1);
	quickSortStable(middle2, last);
}

template <class ForwardIt>
void heapSort(ForwardIt first, ForwardIt last)
{
	std::make_heap(first, last);
	std::sort_heap(first, last);
}

template <class ForwardIt>
void selectionSort(ForwardIt first, ForwardIt last)
{
	for(auto it = first; it != last; ++it)
	{
		std::swap(*it, *std::min_element(it, last));
	}
}

std::ostream& operator<<(std::ostream& stream, const glm::vec2& rhs);
std::ostream& operator<<(std::ostream& stream, const glm::vec3& rhs);
std::ostream& operator<<(std::ostream& stream, const glm::vec4& rhs);
