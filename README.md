# Allocator aware vector

This is a vector which is similar to std::vector but not entirely because it is not the intent.
This code is just learning excercise.

## Usage
1. Copy `vector.hpp` and `vector_helpers` folder into your project
2. Use `vector` with namespace `omega`

## Simple example
```cpp
omega::vector<int> vec{ 50, 20, 10 };
vec.emplace_back(30);
vec.push_back(20);
vec.erase(vec.cbegin(), vec.cbegin() + 2);
std::sort(vec.begin(), vec.end());
for(auto value : vec)
{
    std::cout << value << std::endl;
}
```

## Requirements
1. C++11 compiler

## Differences with std::vector
1. There are no constructors with element `count` as first parameter because they are not consistent with initializer list constructor.  Use `resize` if you need to fill-in some amount of elements.
2. No non-member functions
3. No methods `max_size` and `get_allocator`
