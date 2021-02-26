#include "Test.hpp"
#include "Timer.hpp"

#include <iostream> //For std::cout and std::fixed
#include <iomanip>  //For std::setprecision
#include <random>   //For std::random_device, std::mt19937 and std::uniform_int_distribution
#include <sstream>  //For std::stringstream
#include <fstream>  //For std::ofstream
#include <string>   //For std::string

struct Comparison
{
    std::string SortType = "";
    std::string BestTimeName = "";
    std::string BestAverageTimeName = "";
    std::string WorstTimeName = "";
    size_t VectorSize = 0;
    double Best = 0.0;
    double Average = 0.0;
    double Worst = 0.0;
};

static size_t g_ITERATIONS = 10;
static size_t g_ARRAYSIZE = 4; // 1->10, 2->100, 3->1000...
static std::stringstream s_FileBuffer;

static std::vector<Comparison> s_ComparisonVector;
static std::stringstream s_ComparisonBuffer;

static void ClearComparisonFile()noexcept
{
    const std::string filename("data/comparison.txt");
    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::trunc);
    file.close();
}

static void SerializeComparison() noexcept
{
    constexpr size_t precision = 20;

    for (const Comparison& comparison : s_ComparisonVector)
    {
        s_ComparisonBuffer << "Sort Type: " << comparison.SortType << " - Container size: " << comparison.VectorSize << ".\n";
        s_ComparisonBuffer << "\tBest time: " << std::fixed << std::setprecision(precision) << comparison.Best << " (" << comparison.BestTimeName << ").\n";
        s_ComparisonBuffer << "\tBest average time: " << std::fixed << std::setprecision(precision) << comparison.Average << " (" << comparison.BestAverageTimeName << ").\n";
        s_ComparisonBuffer << "\tWorst time: " << std::fixed << std::setprecision(precision) << comparison.Worst << " (" << comparison.WorstTimeName << ").\n\n";
    }

    const std::string filename("data/comparison.txt");
    std::ofstream file(filename, std::ios::out | std::ios::app);
    file << s_ComparisonBuffer.str();
    file.close();
    s_ComparisonBuffer.clear();
    s_ComparisonBuffer.flush();
    s_ComparisonBuffer.str("");
}

static void WriteComparison(const char* sort_name, const char* sort_type, size_t vector_size, double best, double average, double worst) noexcept
{
    Comparison comparison{ sort_type, sort_name, sort_name, sort_name, vector_size, best, average, worst };

    bool found = false;
    for (Comparison& element : s_ComparisonVector)
    {
        if (comparison.SortType == element.SortType && comparison.VectorSize == element.VectorSize)
        {
            found = true;
            if (comparison.Best < element.Best)
            {
                element.Best = comparison.Best;
                element.BestTimeName = comparison.BestTimeName;
            }
            if (comparison.Average < element.Average)
            {
                element.Average = comparison.Average;
                element.BestAverageTimeName = comparison.BestAverageTimeName;
            }
            if (comparison.Worst > element.Worst)
            {
                element.Worst = comparison.Worst;
                element.WorstTimeName = comparison.WorstTimeName;
            }
        }
    }

    if (!found)
    {
        s_ComparisonVector.push_back(comparison);
    }
}

static void SerializeResults(const std::string& path) noexcept
{
    const std::string filename("data/" + path);
    std::ofstream file(filename, std::ios::out | std::ios::app);
    auto& tmp = s_FileBuffer.str().back();
    tmp = 0;
    tmp = s_FileBuffer.str().at(s_FileBuffer.str().size() - 2);
    tmp = 0;
    file << s_FileBuffer.str();
    file.close();
    s_FileBuffer.clear();
    s_FileBuffer.flush();
    s_FileBuffer.str("");
}

static void ClearFile(const std::string& path) noexcept
{
    const std::string filename("data/" + path);
    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::trunc);
    file.close();
}

static void WriteSeparator(const size_t line_size)
{
    for (size_t i = 0; i < line_size; ++i)
    {
        s_FileBuffer << "*";
    }
    s_FileBuffer << std::endl;
}

static void WriteSpaceUntilEndOfLine(const size_t spaces)
{
    for (size_t i = 1; i < spaces; ++i)
    {
        s_FileBuffer << " ";
    }
    s_FileBuffer << "*" << std::endl;
}

static void WriteResults(const char* sort_name, const char* sort_type, size_t vector_size, bool sorted, double best, double average, double worst)
{
    size_t elements_size = 0;
    size_t sort_type_size = strlen(sort_type);
    {
        size_t tmp = vector_size;
        while (tmp != 0)
        {
            ++elements_size;
            tmp /= 10;
        }
    }
    const size_t line_size = elements_size > sort_type_size ?  15 + elements_size : 24 + sort_type_size;
    constexpr size_t precision = 20;

    WriteSeparator(line_size);
    size_t tmp = 2 + strlen(sort_name);
    s_FileBuffer << "* " << sort_name; WriteSpaceUntilEndOfLine(line_size - tmp);
    tmp = 13 + strlen(sort_type);
    s_FileBuffer << "* Sort type: " << sort_type; WriteSpaceUntilEndOfLine(line_size - tmp);
    tmp = 22 + elements_size;
    s_FileBuffer << "* Size of the vector: " << vector_size; WriteSpaceUntilEndOfLine(line_size - tmp);
    WriteSeparator(line_size);
    s_FileBuffer << (sorted ? "Sorted succesfully" : "Sorted failed") << std::endl;
    s_FileBuffer << "Best time:          " << std::fixed << std::setprecision(precision) << best << " seconds" << std::endl;
    s_FileBuffer << "Worst time:         " << std::fixed << std::setprecision(precision) << worst << " seconds" << std::endl;
    s_FileBuffer << "Average time:       " << std::fixed << std::setprecision(precision) << average << " seconds" << std::endl;
    s_FileBuffer << std::endl;
    s_FileBuffer << std::endl;
}

/*
* TEST FUNCTIONS
*/

void Test::RunAllTests() noexcept
{
    ClearComparisonFile();

    RunBubbleSortTest();
    RunSelectionSortTest();
    RunInsertionSortTest();
    RunMergeSortTest();
    RunQuickSortTest();
    RunDefaultSortTest();

    SerializeComparison();
}

void Test::QuickVSDefault() noexcept
{
    g_ITERATIONS = 50;
    g_ARRAYSIZE = 8;

    ClearComparisonFile();

    ClearFile("Quick_Sort.txt");
    ClearFile("Default_Sort.txt");
    ExecuteTest(SortAlgorithm::QuickSort, Type::Random);
    ExecuteTest(SortAlgorithm::Default, Type::Random);

    SerializeComparison();
}

void Test::RunBubbleSortTest() noexcept
{
    ClearFile("Bubble_Sort.txt");
    ExecuteTest(SortAlgorithm::BubbleSort, Type::Random);
    ExecuteTest(SortAlgorithm::BubbleSort, Type::Front);
    ExecuteTest(SortAlgorithm::BubbleSort, Type::Middle);
    ExecuteTest(SortAlgorithm::BubbleSort, Type::Back);
    ExecuteTest(SortAlgorithm::BubbleSort, Type::Reversed);
    ExecuteTest(SortAlgorithm::BubbleSort, Type::Bitonic);
    ExecuteTest(SortAlgorithm::BubbleSort, Type::Rotated);
}

void Test::RunSelectionSortTest() noexcept
{
    ClearFile("Selection_Sort.txt");
    ExecuteTest(SortAlgorithm::SelectionSort, Type::Random);
    ExecuteTest(SortAlgorithm::SelectionSort, Type::Front);
    ExecuteTest(SortAlgorithm::SelectionSort, Type::Middle);
    ExecuteTest(SortAlgorithm::SelectionSort, Type::Back);
    ExecuteTest(SortAlgorithm::SelectionSort, Type::Reversed);
    ExecuteTest(SortAlgorithm::SelectionSort, Type::Bitonic);
    ExecuteTest(SortAlgorithm::SelectionSort, Type::Rotated);
}

void Test::RunInsertionSortTest() noexcept
{
    ClearFile("Insertion_Sort.txt");
    ExecuteTest(SortAlgorithm::InsertionSort, Type::Random);
    ExecuteTest(SortAlgorithm::InsertionSort, Type::Front);
    ExecuteTest(SortAlgorithm::InsertionSort, Type::Middle);
    ExecuteTest(SortAlgorithm::InsertionSort, Type::Back);
    ExecuteTest(SortAlgorithm::InsertionSort, Type::Reversed);
    ExecuteTest(SortAlgorithm::InsertionSort, Type::Bitonic);
    ExecuteTest(SortAlgorithm::InsertionSort, Type::Rotated);
}

void Test::RunMergeSortTest() noexcept
{
    ClearFile("Merge_Sort.txt");
    ExecuteTest(SortAlgorithm::MergeSort, Type::Random);
    ExecuteTest(SortAlgorithm::MergeSort, Type::Front);
    ExecuteTest(SortAlgorithm::MergeSort, Type::Middle);
    ExecuteTest(SortAlgorithm::MergeSort, Type::Back);
    ExecuteTest(SortAlgorithm::MergeSort, Type::Reversed);
    ExecuteTest(SortAlgorithm::MergeSort, Type::Bitonic);
    ExecuteTest(SortAlgorithm::MergeSort, Type::Rotated);
}

void Test::RunQuickSortTest() noexcept
{
    ClearFile("Quick_Sort.txt");
    ExecuteTest(SortAlgorithm::QuickSort, Type::Random);
    ExecuteTest(SortAlgorithm::QuickSort, Type::Front);
    ExecuteTest(SortAlgorithm::QuickSort, Type::Middle);
    ExecuteTest(SortAlgorithm::QuickSort, Type::Back);
    ExecuteTest(SortAlgorithm::QuickSort, Type::Reversed);
    ExecuteTest(SortAlgorithm::QuickSort, Type::Bitonic);
    ExecuteTest(SortAlgorithm::QuickSort, Type::Rotated);
}

void Test::RunDefaultSortTest() noexcept
{
    ClearFile("Default_Sort.txt");
    ExecuteTest(SortAlgorithm::Default, Type::Random);
    ExecuteTest(SortAlgorithm::Default, Type::Front);
    ExecuteTest(SortAlgorithm::Default, Type::Middle);
    ExecuteTest(SortAlgorithm::Default, Type::Back);
    ExecuteTest(SortAlgorithm::Default, Type::Reversed);
    ExecuteTest(SortAlgorithm::Default, Type::Bitonic);
    ExecuteTest(SortAlgorithm::Default, Type::Rotated);
}

void Test::ExecuteTest(SortAlgorithm algorithm, Test::Type test_type) noexcept
{
    size_t vector_size = 1;
    for (size_t i = 0; i < g_ARRAYSIZE; ++i)
    {
        vector_size *= 10;
        bool sorted = true;
        double best = INFINITY;
        double average = 0.0;
        double worst = 0.0;
        std::string type = "";
        switch (test_type)
        {
        case Type::Random:
            std::cout << "Random Test with size: " << vector_size << std::endl;
            type = "Randomized Vector";
            break;
        case Type::Front:
            std::cout << "Almost Sorted (Front) Test with size: " << vector_size << std::endl;
            type = "Almost Sorted (Front) Vector";
            break;
        case Type::Middle:
            std::cout << "Almost Sorted (Middle) Test with size: " << vector_size << std::endl;
            type = "Almost Sorted (Middle) Vector";
            break;
        case Type::Back:
            std::cout << "Almost Sorted (Back) Test with size: " << vector_size << std::endl;
            type = "Almost Sorted (Back) Vector";
            break;
        case Type::Reversed:
            std::cout << "Reversed Test with size: " << vector_size << std::endl;
            type = "Reversed Vector";
            break;
        case Type::Bitonic:
            std::cout << "Bitonic Test with size: " << vector_size << std::endl;
            type = "Bitonic Vector";
            break;
        case Type::Rotated:
            std::cout << "Rotated Test with size: " << vector_size << std::endl;
            type = "Rotated Vector";
        }

        for (size_t j = 0; j < g_ITERATIONS && sorted; ++j)
        {
            std::cout << "\tIteration: " << j << std::endl;
            std::vector<size_t> vector;
            vector.reserve(vector_size);
            switch (test_type)
            {
            case Type::Random: FillRandom(vector, vector_size); break;
            case Type::Front: FillAmostSortedFront(vector, vector_size); break;
            case Type::Middle: FillAmostSortedMiddle(vector, vector_size); break;
            case Type::Back: FillAmostSortedBack(vector, vector_size); break;
            case Type::Reversed: FillReversed(vector, vector_size); break;
            case Type::Bitonic: FillBitonic(vector, vector_size); break;
            case Type::Rotated: FillRotated(vector, vector_size); break;
            }
            Timer timer;
            timer.Start();
            Sort(vector.begin(), vector.end(), std::greater<size_t>(), algorithm);
            double time = timer.Stop();
            average += time;
            if (time < best) { best = time; }
            if (time > worst) { worst = time; }
            sorted = CheckVector(vector);
            if (!sorted) { std::cout << "Test failed!" << std::endl; }
        }
        average /= g_ITERATIONS;
        switch (algorithm)
        {
        case SortAlgorithm::Default:
            WriteComparison("Default Sort", type.c_str(), vector_size, best, average, worst);
            WriteResults("Default Sort", type.c_str(), vector_size, sorted, best, average, worst);
            SerializeResults("Default_Sort.txt");
            break;
        case SortAlgorithm::BubbleSort:
            WriteComparison("Bubble Sort", type.c_str(), vector_size, best, average, worst);
            WriteResults("Bubble Sort", type.c_str(), vector_size, sorted, best, average, worst);
            SerializeResults("Bubble_Sort.txt");
            break;
        case SortAlgorithm::SelectionSort:
            WriteComparison("Selection Sort", type.c_str(), vector_size, best, average, worst);
            WriteResults("Selection Sort", type.c_str(), vector_size, sorted, best, average, worst);
            SerializeResults("Selection_Sort.txt");
            break;
        case SortAlgorithm::InsertionSort:
            WriteComparison("Insertion Sort", type.c_str(), vector_size, best, average, worst);
            WriteResults("Insertion Sort", type.c_str(), vector_size, sorted, best, average, worst);
            SerializeResults("Insertion_Sort.txt");
            break;
        case SortAlgorithm::MergeSort:
            WriteComparison("Merge Sort", type.c_str(), vector_size, best, average, worst);
            WriteResults("Merge Sort", type.c_str(), vector_size, sorted, best, average, worst);
            SerializeResults("Merge_Sort.txt");
            break;
        case SortAlgorithm::QuickSort:
            WriteComparison("Quick Sort", type.c_str(), vector_size, best, average, worst);
            WriteResults("Quick Sort", type.c_str(), vector_size, sorted, best, average, worst);
            SerializeResults("Quick_Sort.txt");
            break;
        }
    }
}

/*
* VECTOR FUNCTIONS
*/

void Test::FillRandom(std::vector<size_t>& vector, size_t size) noexcept
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<size_t> dist(0, size * 10);

    for (size_t i = 0; i < size; ++i)
    {
        const size_t random_value = dist(mt);
        vector.push_back(random_value);
    }
}

void Test::FillAmostSortedFront(std::vector<size_t>& vector, size_t size) noexcept
{
    //9,1,2,3,4,5,6,7,8
    vector.push_back(size);

    for (size_t i = 1; i < size; ++i)
    {
        vector.push_back(i);
    }
}

void Test::FillAmostSortedMiddle(std::vector<size_t>& vector, size_t size) noexcept
{
    //1,2,3,4,9,5,6,7,8
    size_t i = 0;
    for (; i < (size >> 1); ++i)
    {
        vector.push_back(i);
    }
    vector.push_back(size);
    for (; i < size; ++i)
    {
        vector.push_back(i);
    }
}

void Test::FillAmostSortedBack(std::vector<size_t>& vector, size_t size) noexcept
{
    //1,2,3,4,5,6,7,9,8
    for (size_t i = 0; i < size; ++i)
    {
        vector.push_back(i);
    }

    size_t tmp = vector.at(size - 2);
    vector.at(size - 2) = vector.back();
    vector.back() = tmp;
}

void Test::FillReversed(std::vector<size_t>& vector, size_t size) noexcept
{
    //9,8,7,6,5,4,3,2,1
    size_t value = size - 1;
    for (size_t i = 0; i < size; ++i, --value)
    {
        vector.push_back(value);
    }
}

void Test::FillBitonic(std::vector<size_t>& vector, size_t size) noexcept
{
    //0,1,2,3,4,9,8,7,6,5
    const size_t middle = size / 2;
    for (size_t i = 0; i < size; ++i)
    {
        if (i < middle) { vector.push_back(i); }
        else { vector.push_back(size - i); }
    }
}

void Test::FillRotated(std::vector<size_t>& vector, size_t size) noexcept
{
    //0,9,1,8,2,7,3,6,4,5
    size_t value = size;
    for (size_t i = 0; i < size; ++i)
    {
        if (i % 2 == 0) { vector.push_back(i); }
        else { vector.push_back(--value); }
    }
}
