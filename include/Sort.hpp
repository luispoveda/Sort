#pragma once

/*
* Author: Luis Poveda Cano.
* luispovedacano@gmail.com
*/

/*
* Sort algorithms implementation header.
* 
* The implemented algorithms are the following:
* Bubble sort
* Selection sort
* Insertion sort
* Merge sort
* Quick sort
*/

/*
MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Copyright (c) 2021 Luis Poveda Cano
*/

#include <iterator>     //For std::advance, std::prev, std::distance
#include <algorithm>    //For std::iter_swap
#include <vector>       //For std::vector

enum class SortAlgorithm : unsigned char
{
    Default,
    BubbleSort,
    SelectionSort,
    InsertionSort,
    MergeSort,
    QuickSort
};

template<typename Iterator, typename Comparator>
class Sort
{
    using IteratorType = typename std::iterator_traits<Iterator>::value_type;
public:
    Sort(Iterator begin, Iterator end, Comparator comparator, SortAlgorithm algorithm = SortAlgorithm::Default) noexcept :
        m_Begin(begin),
        m_End(end)
    {
        Run(comparator, algorithm);
    }

    //Sort implementations
private:
    /*
    * Bubble sort works by repeatedly swapping the adjacent elements if needed.
    * The algorithm keeps running until in one of the iterations does not find any element to swap.
    * Stable sort.
    * 
    * Time complexity:
    * Best: O(n)
    * Worst: O(n^2)
    * Average: O(n^2)
    * Space complexity: O(1)
    */
    void BubbleSort(Iterator begin, Iterator end, Comparator comparator) noexcept
    {
        bool sorted = false;
        while(!sorted)
        {
            sorted = true;
            Iterator current = begin;
            Iterator next = begin;
            for (std::advance(next, 1); next != end; std::advance(current, 1), std::advance(next, 1))
            {
                if (comparator(*current, *next))
                {
                    sorted = false;
                    std::iter_swap(current, next);
                }
            }
            std::advance(end, -1);
        };
    }

    /*
    * Selection sort works by searching in each iteration for the minimum or maximum value, depending on whether the sort is ascending or descending and swapping it with the pivot position.
    * The pivot position starts at the beginning of the container and increases by one in each iteration.
    * Stable sort.
    * 
    * Time complexity:
    * Best: O(n^2)
    * Worst: O(n^2)
    * Average: O(n^2)
    * Space complexity: O(1)
    */
    void SelectionSort(Iterator begin, Iterator end, Comparator comparator) noexcept
    {
        Iterator last = end;
        Iterator pivot = begin;
        for (std::advance(last, -1); pivot != last; std::advance(pivot, 1))
        {
            bool found = false;
            Iterator min = pivot;
            Iterator value = pivot;
            for (std::advance(value, 1); value != end; std::advance(value, 1))
            {
                if (comparator(*min, *value))
                {
                    found = true;
                    min = value;
                }
            }
            if (found) { std::iter_swap(pivot, min); }
        }
    }

    /*
    * Insertion sort works by comparing the pivot position with all the previous ones and swapping the positions if needed.
    * The pivot position starts at the second position of the container and increases by one in each iteration.
    * Stable sort
    * 
    * Time complexity:
    * Best: O(n)
    * Worst: O(n^2)
    * Average: O(n^2)
    * Space complexity: O(1)
    */
    void InsertionSort(Iterator begin, Iterator end, Comparator comparator) noexcept
    {
        Iterator pivot = begin;
        for (std::advance(pivot, 1); pivot != end; std::advance(pivot, 1))
        {
            Iterator current = pivot;
            Iterator prev = pivot;
            IteratorType pivot_value = std::move(*pivot); //Store the pivot value to avoid unnecesary moves and copy it to its place at the end of the iteration.

            do
            {
                std::advance(prev, -1);
                if (!comparator(*prev, pivot_value)) { break; }
                *current = std::move(*prev);
                std::advance(current, -1);
            } while (prev != begin);

            *current = std::move(pivot_value);
        }
    }

    /*
    * Merge sort is a divide and conquer algorithm.
    * For each pass, the vector is divided by the middle into left vector and right vector.
    * This recursive process keeps until the partitions size is 1.
    * Once the divide process has finished, starts to merge those partitions and sort them in the process.
    * In this implementation, for merge the partitions, std::vectors are created.
    * Stable sort
    *
    * Time complexity:
    * Best: O(n log n)
    * Worst: O(n log n)
    * Average: O(n log n)
    * Space complexity: O(n)
    */
    void MergeSort(Iterator begin, Iterator end, Comparator comparator) noexcept
    {
        std::advance(end, -1);
        _MergeSortImp(begin, end, comparator);
    }

    /*
    * Quick sort is a divide and conquer algorithm.
    * For each pass, the vector is divided in two parts, the left part is lesser than the pivot value and the right part is greater than the pivot value.
    * In this implementation, the pivot value is the median of 3 values.
    * These are the element at 1/4, the element at 1/2 and the element at 3/4 of the container.
    * This recursive process keeps until the container is sorted.
    * Not stable sort
    *
    * Time complexity:
    * Best: O(n log n)
    * Worst: O(n^2)
    * Average: O(n log n)
    * Space complexity: O(1)
    */
    void QuickSort(Iterator begin, Iterator end, Comparator comparator) noexcept
    {
        _QuickSortImp(begin, end, comparator);
    }

    /*
    * Default Sort is an optimized version of QuickSort.
    * If the number of elements to sort is less than 200, then perform an InsertionSort.
    * Otherwise, perform an optimized QuickSort.
    * This optimized QuickSort consists in splitting the container but once the container has a certain size, sort it by InsertionSort instead of splitting it further.
    *
    * Time complexity:
    * Best: O(n log n)
    * Worst: O(n^2)
    * Average: O(n log n)
    * Space complexity: O(1)
    */
    void DefaultSort(Iterator begin, Iterator end, Comparator comparator) noexcept
    {
        const size_t size = std::distance(begin, end);
        if (size <= 200)
        {
            InsertionSort(begin, end, comparator);
        }
        else
        {
            _DefaultSortImp(begin, end, comparator);
        }
    }

    //Internal functions
private:
    //Merge Sort internal.
    void _MergeSortImp(Iterator left, Iterator right, Comparator comparator) noexcept
    {
        const size_t left_distance = std::distance(m_Begin, left);
        const size_t right_distance = std::distance(m_Begin, right);

        if (left_distance >= right_distance) { return; }

        const size_t middle_distance = (left_distance + right_distance - 1) / 2;
        Iterator middle = m_Begin;
        std::advance(middle, middle_distance);
        _MergeSortImp(left, middle, comparator);
        _MergeSortImp(std::next(middle), right, comparator);
        _MergeSubData(left, middle, right, comparator);
    }

    void _MergeSubData(Iterator left, Iterator middle, Iterator right, Comparator comparator) noexcept
    {
        const size_t vector_left_size = std::distance(left, middle) + 1;
        const size_t vector_right_size = std::distance(middle, right);

        std::vector<typename Iterator::value_type> left_vector;
        std::vector<typename Iterator::value_type> right_vector;
        left_vector.reserve(vector_left_size);
        right_vector.reserve(vector_right_size);

        Iterator aux = left;
        for (size_t i = 0; i < vector_left_size; ++i, std::advance(aux, 1))
        {
            left_vector.push_back(*aux);
        }
        aux = middle;
        std::advance(aux, 1);
        for (size_t i = 0; i < vector_right_size; ++i, std::advance(aux, 1))
        {
            right_vector.push_back(*aux);
        }

        Iterator left_vector_iterator = left_vector.begin();
        Iterator right_vector_iterator = right_vector.begin();
        aux = left;
        while (left_vector_iterator != left_vector.end() && right_vector_iterator != right_vector.end())
        {
            if (comparator(*right_vector_iterator, *left_vector_iterator))
            {
                *aux = *left_vector_iterator;
                std::advance(left_vector_iterator, 1);
            }
            else
            {
                *aux = *right_vector_iterator;
                std::advance(right_vector_iterator, 1);
            }
            std::advance(aux, 1);
        }

        while (left_vector_iterator != left_vector.end())
        {
            *aux = *left_vector_iterator;
            std::advance(left_vector_iterator, 1);
            std::advance(aux, 1);
        }
        while (right_vector_iterator != right_vector.end())
        {
            *aux = *right_vector_iterator;
            std::advance(right_vector_iterator, 1);
            std::advance(aux, 1);
        }
    }

    //Quick Sort internal.
    void _QuickSortImp(Iterator left, Iterator right, Comparator comparator) noexcept
    {
        const size_t left_distance = std::distance(m_Begin, left);
        const size_t right_distance = std::distance(m_Begin, right);
        if (left_distance >= right_distance) { return; }

        const Iterator pivot_iterator = _QuickSortPartition(left, right, comparator);
        if (pivot_iterator != m_Begin) { _QuickSortImp(left, std::prev(pivot_iterator), comparator); }
        if (pivot_iterator != std::prev(m_End)) { _QuickSortImp(std::next(pivot_iterator), right, comparator); }
    }

    Iterator _QuickSortPartition(Iterator left, Iterator right, Comparator comparator) noexcept
    {
        Iterator pivot_value = m_Begin;
        Iterator pivot_index = left;
        const size_t half = std::distance(left, right) / 2;
        std::advance(pivot_value,half);
        const size_t quarter = half / 2;
        Iterator left_quarter = std::next(left, quarter);
        Iterator right_quarter = std::prev(right, quarter);

        if (comparator(*left_quarter, *pivot_value))
        {
            if (comparator(*right_quarter, *pivot_value))
            {
                if (comparator(*left_quarter, *right_quarter))
                {
                    pivot_value = right_quarter;
                }
                else
                {
                    pivot_value = left_quarter;
                }
            }
        }
        else
        {
            if (comparator(*pivot_value, *right_quarter))
            {
                if (comparator(*right_quarter, *left_quarter))
                {
                    pivot_value = right_quarter;
                }
                else
                {
                    pivot_value = left_quarter;
                }
            }
        }

        for (; left != pivot_value; std::advance(left, 1))
        {
            if (comparator(*pivot_value, *left))
            {
                std::iter_swap(left, pivot_index);
                std::advance(pivot_index, 1);
            }
        }
        std::iter_swap(pivot_index, pivot_value);
        pivot_value = pivot_index;
        pivot_index = right;
        for (; right != pivot_value; std::advance(right, -1))
        {
            if (comparator(*right, *pivot_value))
            {
                std::iter_swap(right, pivot_index);
                std::advance(pivot_index, -1);
            }
        }
        std::iter_swap(pivot_index, pivot_value);

        return pivot_index;
    }

    //Default Sort internal.
    void _DefaultSortImp(Iterator left, Iterator right, Comparator comparator) noexcept
    {
        const size_t left_distance = std::distance(m_Begin, left);
        const size_t right_distance = std::distance(m_Begin, right);
        if (left_distance >= right_distance) { return; }
        if (right_distance - left_distance <= 200)
        {
            InsertionSort(left, std::next(right), comparator);
            return;
        }

        const Iterator pivot_iterator = _QuickSortPartition(left, right, comparator);
        if (pivot_iterator != m_Begin) { _DefaultSortImp(left, std::prev(pivot_iterator), comparator); }
        if (pivot_iterator != std::prev(m_End)) { _DefaultSortImp(std::next(pivot_iterator), right, comparator); }
    }

    inline void Run(Comparator comparator, SortAlgorithm algorithm) noexcept
    {
        switch (algorithm)
        {
        case SortAlgorithm::Default:
            DefaultSort(m_Begin, m_End, comparator);
            break;
        case SortAlgorithm::BubbleSort:
            BubbleSort(m_Begin, m_End, comparator);
            break;
        case SortAlgorithm::SelectionSort:
            SelectionSort(m_Begin, m_End, comparator);
            break;
        case SortAlgorithm::InsertionSort:
            InsertionSort(m_Begin, m_End, comparator);
            break;
        case SortAlgorithm::MergeSort:
            MergeSort(m_Begin, m_End, comparator);
            break;
        case SortAlgorithm::QuickSort:
            QuickSort(m_Begin, m_End, comparator);
            break;
        }
    }

private:
    Iterator m_Begin;
    Iterator m_End;
};
