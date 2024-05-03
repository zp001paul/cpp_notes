#include "myconcept.hpp"
#include "myprt.hpp"

template <typename Tm, typename Tk>
  requires AllMapType<Tm>
bool node_swap(Tm &m, Tk k1, Tk k2) {
  auto node1{m.extract(k1)};
  auto node2{m.extract(k2)};
  // 其实就是删除掉k1 ,
  // k2两个key，只不过node1和node2保留了原来tree的位置的信息
  PrtMap(m);
  if (node1.empty() || node2.empty()) {
    return false;
  }
  std::swap(node1.key(), node2.key());
  if (!m.insert(std::move(node1)).inserted) {
    return false;
  }
  if (!m.insert(std::move(node2)).inserted) {
    return false;
  }
  return true;
}
int main(int argc, char *argv[]) {
  // map
  std::map<int, std::string> racers{
      {1, "Mario"}, {2, "Luigi"}, {3, "Bower"}, {4, "Peach"}};
  PrtMap(racers);
  node_swap(racers, 2, 4);
  PrtMap(racers);

  // unordered_map ，虽然能通过编译和连接，但是运行结果是不对的
  std::unordered_map<int, std::string> unordered_racers{
      {1, "Mario"}, {2, "Luigi"}, {3, "Bower"}, {4, "Peach"}};
  PrtMap(unordered_racers);
  node_swap(unordered_racers, 2, 4);
  PrtMap(unordered_racers);
  return 0;
}
