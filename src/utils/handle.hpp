#pragma once

namespace cppython {

class object;
class oop_closure;

class handle {
public:
  handle(object *t);
  ~handle();

  void oops_do(oop_closure *f);
  object *operator->();
  object *operator()() { return value; }
  object *resolve();

  handle *get_next() { return next; }

private:
  object *value;
  handle *next;
};

class handle_mark {
public:
  static handle_mark *get_instance();

private:
  handle_mark() = default;
  ~handle_mark() = default;

public:
  handle_mark(const handle_mark &) = delete;
  handle_mark(handle_mark &&) = delete;
  handle_mark &operator=(const handle_mark &) = delete;
  handle_mark &operator=(handle_mark &&) = delete;

public:
  void oops_do(oop_closure *f);
  handle *get_head() { return head; }
  void set_head(handle *x) { head = x; }

private:
  handle *head{nullptr};
};

} // namespace cppython
