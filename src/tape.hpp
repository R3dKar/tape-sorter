#pragma once

namespace tape_sorter {
  template <typename T>
  class ITape {
  public:
    virtual size_t size() const = 0;
    virtual size_t pos() const = 0;

    virtual void shift_forward() = 0;
    virtual void shift_backward() = 0;

    virtual void rewind_forward() = 0;
    virtual void rewind_backward() = 0;

    virtual void write(const T& value) = 0;
    virtual T read() = 0;

    virtual ~ITape() = default;
  };
} // namespace tape_sorter
