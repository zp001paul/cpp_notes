#pragma once

class UnsyncBuf {
public:
  void put(int value) { m_buf = value; }
  int get() const { return m_buf; }

private:
  int m_buf{-1};
};
