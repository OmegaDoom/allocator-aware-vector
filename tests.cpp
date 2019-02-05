#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <string>
#include <tuple>
#include <stdexcept>
#include "vector.hpp"

class Test
{
    friend std::ostream& operator << (std::ostream&, const Test&);
public:
    Test() = default;
    explicit Test(const std::string& str)
        : m_str(str)
    {
    }

    Test(const Test& rhs)
        : m_str(rhs.m_str)
    {
    }

    bool operator == (const Test& rhs) const
    {
        return m_str == rhs.m_str;
    }

    bool operator == (const std::string& str) const
    {
        return m_str == str;
    }

private:
    std::string m_str;
};

std::ostream& operator << (std::ostream& os, const Test& other)
{
    os << other.m_str;
    return os;
}

TEMPLATE_TEST_CASE( "vectors can be sized and resized", "[vector][template]", int, std::string, Test, (std::tuple<int,float>) )
{
  omega::vector<TestType> v;
  v.resize(5);

  REQUIRE( v.size() == 5 );
  REQUIRE( v.capacity() >= 5 );

  SECTION( "resizing bigger changes size and capacity" ) {
      v.resize( 10 );

      REQUIRE( v.size() == 10 );
      REQUIRE( v.capacity() >= 10 );
  }
  SECTION( "resizing smaller changes size but not capacity" ) {
      v.resize( 0 );

      REQUIRE( v.size() == 0 );
      REQUIRE( v.capacity() >= 5 );

      SECTION( "We can use the 'swap trick' to reset the capacity" ) {
          omega::vector<TestType> empty;
          empty.swap( v );

          REQUIRE( v.capacity() == 0 );
      }
  }
  SECTION( "reserving smaller does not change size or capacity" ) {
      v.reserve( 0 );

      REQUIRE( v.size() == 5 );
      REQUIRE( v.capacity() >= 5 );
  }
  SECTION( "reserving bigger changes capacity but not size" ) {
      v.reserve( 50 );

      REQUIRE( v.size() == 5 );
      REQUIRE( v.capacity() >= 50 );
  }
}

TEST_CASE( "initialization", "[vector]" ) {

    std::vector<int> init_from { 4, 5, 6 };

    SECTION( "initializer_list" ) {
        omega::vector<int> v { 0, 1, 2 };
        REQUIRE( v.size() == 3 );
        REQUIRE( (v[0] == 0 && v[1] == 1 && v[2] == 2) );
    }

    SECTION( "const initialization from vector" ) {
        omega::vector<int> v ( init_from.cbegin(), init_from.cend());
        REQUIRE( v.size() == 3 );
        REQUIRE( (v[0] == 4 && v[1] == 5 && v[2] == 6) );
    }

    SECTION( "initialization from vector" ) {
        omega::vector<int> v ( init_from.begin(), init_from.end());
        REQUIRE( v.size() == 3 );
        REQUIRE( (v[0] == 4 && v[1] == 5 && v[2] == 6) );
    }
}

TEST_CASE( "construction", "[vector]" ) {
    omega::vector<int> init_v { 0, 1, 2 };
    SECTION( "copy construction" ) {
        omega::vector<int> v(init_v); 
        REQUIRE( v.size() == 3 );
        REQUIRE( (v[0] == 0 && v[1] == 1 && v[2] == 2) );
    }
    omega::vector<int> v { 0, 1, 2 };
    SECTION( "move constructon" ) {
        omega::vector<int> other(std::move(init_v)); 
        REQUIRE( v.size() == 3 );
        REQUIRE( (v[0] == 0 && v[1] == 1 && v[2] == 2) );
    }
}

SCENARIO( "Can add items", "[vector]" ) {

    GIVEN( "A vector with one item" ) {
        omega::vector<int> v;
        v.push_back(10);

        REQUIRE( v[0] == 10 );

        WHEN( "add another item" ) {
            v.push_back( 11 );

            THEN( "the last item equals 11" ) {
                REQUIRE( v.back() == 11 );
            }
        }
        WHEN( "insert an item at the begining" ) {
            v.insert(v.begin(), 9);

            THEN( "the first item equals 9" ) {
                REQUIRE( v.front() == 9 );
            }
        }
        WHEN( "insert an item at the end" ) {
            v.insert(v.end(), 12);

            THEN( "the last item equals 12" ) {
                REQUIRE( v.back() == 12 );
            }
        }
        WHEN( "insert complex items" ) {
            omega::vector<Test> vec;
            vec.insert(vec.cend(), Test("qwerty"));
            vec.insert(vec.cbegin(), 2, Test("second"));
            const Test item{"first"};
            vec.insert(vec.cbegin(), 2, item);
            vec.insert(vec.cbegin(), item);
            THEN( "first item is first, fourth is second, sixth is qwerty" ) {
                REQUIRE( (vec[0] == "first" && vec[3] == "second" && vec[5] == "qwerty") );
            }
        }
        WHEN( "insert initializer list" ) {
            v.insert(v.cbegin(), { 8, 9 });

            THEN( "items are 8, 9, 10" ) {
                REQUIRE( (v[0] == 8 && v[1] == 9 && v[2] == 10) );
            }
        }
        WHEN( "insert iterators" ) {
            std::vector<int> insert_v { 6, 7 };
            v.insert(v.cbegin(), insert_v.cbegin(), insert_v.cend());
            std::vector<int> insert_v2 { 8, 9 };
            v.insert(v.cbegin() + 2, insert_v2.cbegin(), insert_v2.cend());

            THEN( "items are 6, 7, 8, 9, 10" ) {
                REQUIRE( (v[0] == 6 && v[1] == 7 && v[2] == 8 && v[3] == 9 && v[4] == 10) );
            }
        }
        WHEN( "adding items by resizing" ) {
            v.resize(3, 5);
            THEN( "items are 10, 5, 5" ) {
                REQUIRE( (v[0] == 10 && v[1] == 5 && v[2] == 5) );
            }
        }
    }
}

SCENARIO( "Emplace add items", "[vector]" ) {

    GIVEN( "A vector with one item" ) {
        omega::vector<Test> v;
        v.emplace_back("123");

        REQUIRE( v[0] == "123" );

        WHEN( "add another item" ) {
            v.emplace_back( "abc" );

            THEN( "the last item equals abc" ) {
                REQUIRE( v.back() == "abc" );
            }
        }
        WHEN( "insert an item at the begining" ) {
            v.emplace(v.begin(), "zxc");
            THEN( "the first item equals zxc" ) {
                REQUIRE( v.front() == "zxc" );
            }
        }
        WHEN( "insert an item at the end" ) {
            v.emplace(v.end(), "asd");

            THEN( "the last item is equal to asd" ) {
                REQUIRE( v.back() == "asd" );
            }
        }
    }
}

SCENARIO( "assign items", "[vector]" ) {

    GIVEN( "A vector with some items" ) {
        omega::vector<std::string> v { "1", "2", "3" };

        WHEN( "list inializer assign" ) {
            v.assign({ "4", std::string("5"), std::string("6") });

            THEN( "items are equal to 4, 5, 6" ) {
                REQUIRE( (v.size() == 3 && v[0] == "4" && v[1] == "5" && v[2] == "6") );
            }
        }

        WHEN( "assign from iterators" ) {
            std::vector<std::string> v_from { "a", "b", "c" };
            v.assign(v_from.cbegin(), v_from.cend());

            THEN( "items are equal to a, b, c" ) {
                REQUIRE( (v.size() == 3 && v[0] == "a" && v[1] == "b" && v[2] == "c") );
            }
        }
    }
}

SCENARIO( "Shrink to fit", "[vector]" ) {

    GIVEN( "A vector with some items" ) {
        omega::vector<int> v;
        v.push_back(10);
        v.push_back(11);
        v.push_back(12);
        WHEN( "add another item" ) {
            v.push_back( 13 );
            v.shrink_to_fit();

            THEN( "the cpacity is 4") {
                REQUIRE( v.capacity() == 4 );
            }
        }
        WHEN( "add another items" ) {
            v.push_back( 13 );
            v.push_back( 14 );
            v.push_back( 15 );
            v.shrink_to_fit();

            THEN( "the capacity is 6") {
                REQUIRE( v.capacity() == 6 );
            }
        }
    }
}

SCENARIO( "delete items", "[vector]" ) {

    GIVEN( "A vector with some items" ) {
        omega::vector<int> v;
        v.push_back(10);
        v.push_back(11);
        v.push_back(12);
        WHEN( "delete item" ) {
            v.pop_back();

            THEN( "the last item is 11") {
                REQUIRE( v.back() == 11 );
            }
        }
        WHEN( "delete item" ) {
            v.erase(v.cbegin() + 1);

            THEN( "the second item is 12") {
                REQUIRE( v.back() == 12 );
            }
        }
        WHEN( "delete items from end" ) {
            v.erase(v.cbegin() + 1, v.cend());

            THEN( "only one item that equals 10") {
                REQUIRE( (v.size() == 1 && v[0] == 10) );
            }
        }
        WHEN( "delete items by resizing to smaller size with value" ) {
            v.resize(2, 5);

            THEN( "two items left") {
                REQUIRE( (v.size() == 2 && v[0] == 10 && v[1] == 11) );
            }
        }
    }
}

TEST_CASE( "at method", "[vector]" ) {
    omega::vector<int> v;
    v.push_back(10);
    v.push_back(11);
    v.push_back(12);
    SECTION( "index out of range" ) {
        REQUIRE_THROWS_AS(v.at(10) == 0, std::out_of_range);
        REQUIRE_THROWS_AS(v.at(-10) == 0, std::out_of_range);
    }
}
