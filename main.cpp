#include "vector.hpp"
#include "allocator.hpp"

#include <iostream>
#include <string>
#include <algorithm>

class Test
{
    friend std::ostream& operator << (std::ostream&, const Test&);
public:
    Test() = default;
    Test(const std::string& str)
        : m_str(str)
    {
        //std::cout << "ctor" << std::endl;
    }

    Test(const Test& rhs)
        : m_str(rhs.m_str)
    {
       //std::cout << "cctor" << std::endl;
    }

    //Test(Test&& rhs)
    //    : m_str(std::move(rhs.m_str))
    //{
    //    //std::cout << "move ctor" << std::endl;
    //}

    bool operator == (const Test& rhs) const
    {
        return m_str == rhs.m_str;
    }

private:
    std::string m_str;
};

class Test2
{
public:
    Test2(int a, int b)
        : m_a(a), m_b(b)
    {
    }
private:
    int m_a;
    int m_b;
};

std::ostream& operator << (std::ostream& os, const Test& other)
{
    os << other.m_str; 
    return os;
}

int main(int, char* [])
{
    omega::vector<std::string, allocator<std::string>> v5;
    v5.push_back("12345");
    omega::vector<Test2> vec4;
    vec4.emplace_back(1, 2);

    omega::vector<Test> vec;
    vec.emplace_back("qwerty");
    vec.emplace_back("qwerty2");
    omega::vector<Test> vec8;
    vec8 = vec;

    for (auto& item : vec8)
    {
        std::cout << item << " ";
    }

    std::cout << std::endl;

    Test test1("hello!");
    Test test2("by!");
    Test test3("test!");
    vec.push_back(test1);
    vec.push_back(std::string("12345"));
    vec.push_back(test2);
    vec.push_back(std::string("12345"));
    vec.push_back(test3);
    vec.push_back(std::string("12345"));
    vec.push_back(std::string("12345"));
    auto iter2 = vec.insert(vec.end(), std::string("first"));
    std::cout << "inserted item = " << *iter2 << std::endl;
    vec.emplace(vec.begin(), "second");

    for (auto& item : vec)
    {
        std::cout << item << " ";
    }

    std::cout << std::endl;

    vec.erase(vec.end());
    vec.shrink_to_fit();

    auto it_remove = std::remove_if(vec.begin(), vec.end()
                                  , [] (const Test& item) { return item == std::string("12345"); });
    vec.erase(it_remove, vec.end());

    omega::vector<Test> vec1(std::move(vec));
    omega::vector<Test> vec2;
    vec2 = std::move(vec1);

    std::cout << "capacity = " << vec2.capacity() << std::endl;

    auto found_item = std::find(vec2.cbegin(), vec2.cend(), Test(std::string("test!")));
    
    if (found_item != vec2.cend())
        std::cout << "found item = " << *found_item << std::endl;

    omega::vector<int> vec3{10, 5, 3, 7, 8, 0};
    vec3.emplace_back(1);
    std::sort(vec3.begin(), vec3.end());

    vec3.erase(vec3.begin() + 2, vec3.end());
    vec3.emplace_back(2);
    vec3.emplace_back(3);
    vec3.emplace_back(4);

    for (auto iter = vec3.begin(); iter != vec3.end();)
    {
        if (*iter < 3)
            iter = vec3.erase(iter);
        else
            ++iter;
    }

    omega::vector<int> vec5(vec3.cbegin(), vec3.cend());
    vec5.resize(10, 1);

    vec5.insert(vec5.cbegin(), { 1, 2, 3, 4, 5});

    for (auto iter = vec5.rbegin(); iter != vec5.rend(); ++iter)
    {
        std::cout << *iter << " ";
    }

    std::cout << std::endl;

    try
    {
        vec4.at(10);
    }
    catch(const std::out_of_range& exception)
    {
        std::cout << exception.what() << std::endl;
    }

    omega::vector<int> vec6;
    vec6.assign(vec5.cbegin(), vec5.cend());
    for (auto iter = vec6.rbegin(); iter != vec6.rend(); ++iter)
    {
        std::cout << *iter << " "; 
    }

    std::cout << std::endl;
}
