#include <atomic>
#include <cassert>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

using namespace std;

class Cv_Guard {
private:
  condition_variable &con;

public:
  explicit Cv_Guard(condition_variable &con) : con(con) {}

  ~Cv_Guard() { con.notify_one(); }
};

template <typename T> class Channel {
private:
  static constexpr int CHANNEL_SIZE_DEFAULT = 1;
  static constexpr int MICRO_SLEEP_TIME = 1;

  mutex m;
  condition_variable q_not_empty;
  queue<unique_ptr<T>> q;
  const int capacity;
  atomic_flag done;

  [[nodiscard]] int size() const { return q.size(); }

public:
  [[nodiscard]] bool isDone() const { return done.test(); }

  void close() { done.test_and_set(); }

  explicit Channel(const int capacity = CHANNEL_SIZE_DEFAULT)
      : capacity(capacity) {
    assert(capacity > 0);
  }

  friend Channel &operator<<(Channel &ch, unique_ptr<T> data) {
    unique_lock<mutex> lock(ch.m);
    Cv_Guard cvg(ch.q_not_empty);
    while (!ch.isDone() && ch.size() >= ch.capacity) {
      lock.unlock();
      this_thread::sleep_for(chrono::microseconds(MICRO_SLEEP_TIME));
      lock.lock();
    }
    if (ch.isDone()) {
      return ch;
    }
    ch.q.push(move(data));
    return ch;
  }

  friend Channel &operator>>(Channel &ch, unique_ptr<T> &data) {
    unique_lock<mutex> lock(ch.m);
    ch.q_not_empty.wait(lock,
                        [&ch] { return !(ch.q.empty() && !ch.isDone()); });
    if (ch.isDone()) {
      return ch;
    }
    data = std::move(ch.q.front());
    ch.q.pop();
    return ch;
  }
};

void consumer(Channel<int> &chx) {
  unique_ptr<int> data;
  while (!chx.isDone()) {
    chx >> data;
    cout << *data << endl;
  }
}

void producer(Channel<int> &chx) {
  int i = 0;
  while (!chx.isDone()) {
    chx << make_unique<int>(i++);
  }
}

int main() {
  Channel<int> chx(100);
  thread consu(consumer, ref(chx));
  thread pro(producer, ref(chx));
  this_thread::sleep_for(chrono::seconds(1));
  chx.close();
  pro.join();
  consu.join();
}
