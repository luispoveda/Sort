#pragma once

#include "Sort.hpp"
#include <vector>

class Test
{
    enum class Type
    {
        Random,
        Front,
        Middle,
        Back,
        Reversed,
        Bitonic,
        Rotated
    };

public:
    static void RunAllTests() noexcept;
    static void QuickVSDefault() noexcept;
    static void RunBubbleSortTest() noexcept;
    static void RunSelectionSortTest() noexcept;
    static void RunInsertionSortTest() noexcept;
    static void RunMergeSortTest() noexcept;
    static void RunQuickSortTest() noexcept;
    static void RunDefaultSortTest() noexcept;

private:
    template <typename T>
    static void PrintVector(const std::vector<T>& vector) noexcept
    {
        for (auto element : vector)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    template <typename T>
    static bool CheckVector(const std::vector<T>& vector, bool ascending = true) noexcept
    {
        auto next_iterator = vector.cbegin();
        ++next_iterator;
        if (ascending)
        {
            for (auto iterator = vector.cbegin(); next_iterator != vector.cend(); ++iterator, ++next_iterator)
            {
                if (*iterator > *next_iterator) { return false; }
            }
        }
        else
        {
            for (auto iterator = vector.cbegin(); next_iterator != vector.cend(); ++iterator, ++next_iterator)
            {
                if (*iterator < *next_iterator) { return false; }
            }
        }

        return true;
    }

private:
    static void ExecuteTest(SortAlgorithm, Type) noexcept;

private:
    static void FillRandom(std::vector<size_t>&, size_t) noexcept;
    static void FillAmostSortedFront(std::vector<size_t>&, size_t) noexcept;
    static void FillAmostSortedMiddle(std::vector<size_t>&, size_t) noexcept;
    static void FillAmostSortedBack(std::vector<size_t>&, size_t) noexcept;
    static void FillReversed(std::vector<size_t>&, size_t) noexcept;
    static void FillBitonic(std::vector<size_t>&, size_t) noexcept;
    static void FillRotated(std::vector<size_t>&, size_t) noexcept;
};
