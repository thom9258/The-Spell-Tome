#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>

#include <ranges>
//#include <format>

namespace sv = std::views;
namespace sr = std::ranges;


class obj {
public:
  virtual bool init(void) = 0;
  virtual bool update(void) = 0;
  virtual bool destroy(void) = 0;
  virtual bool on_activate(void) {return true;};
  virtual bool on_deactivate(void) {return true;};
};

class newobj : obj {
  bool init(void) {std::cout << "newinit" << std::endl; return true;}
  bool update(void) {std::cout << "newupdate" << std::endl; return true;}
  bool destroy(void) {std::cout << "newdestroy" << std::endl; return true;}
};


class objsystem {
    std::unordered_map<int, std::shared_ptr<obj>> objmap{};
};

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
    
    std::unordered_map<int, std::shared_ptr<obj>> objmap{};
    
    objsystem sysstem;


    auto print = [] (auto v) {
        for (auto x: v) std::cout << x << " ";
        std::cout << std::endl;
    };

    std::vector to5{1, 2, 3, 4, 5};
    print(to5);

    sr::for_each(std::as_const(to5), [] (auto x) { std::cout << x << " ";});
    std::cout << std::endl;

    print(to5 | sv::reverse | std::views::drop(2));
    print(to5 | std::views::take(2));
    print(to5);

	//auto to5t = to5 | sv::transform(std::as_const(to5), [] (auto x) {return x*x;});
    //print(to5t);
}
