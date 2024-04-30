#include <iostream>
#include <string_view>
class Raii {
public:
  Raii(std::string_view name)
      : _name(name){

        };
  Raii(Raii &&) = default;
  Raii(const Raii &) = default;
  Raii &operator=(Raii &&) = default;
  Raii &operator=(const Raii &) = default;
  ~Raii() { std::cout << "dtor() from " << _name << std::endl; }

private:
  std::string _name;
};

int main(int argc, char *argv[]) {
  Raii a{"RaiiA"};
  Raii b{"RaiiB"};
  std::cout << "begin to release\n";
  return 0;
}
