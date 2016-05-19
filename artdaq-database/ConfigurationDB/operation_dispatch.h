#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_OPERATION_DISPATCH_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_OPERATION_DISPATCH_H_

namespace artdaq {
namespace configuration {
namespace dispatch {

template <typename R>
struct Functor {
  virtual R invoke() = 0;
};

template <typename R>
class RFunctor : public Functor<R> {
 public:
  typedef R (*FP)();

  RFunctor(FP fp) : _fp{fp} {}

  R invoke() override final { return _fp(); }

 private:
  FP _fp;
};

template <typename R, typename A1>
class RA1Functor : public Functor<R> {
 public:
  typedef R (*FP)(A1 const&);

  RA1Functor(FP fp, A1 const& a1) : _fp{fp}, _a1{a1} {}

  R invoke() override final { return _fp(_a1); }

 private:
  FP _fp;
  A1 const& _a1;
};

template <typename R, typename A1, typename A2>
class RA1A2Functor : public Functor<R> {
 public:
  typedef R (*FP)(A1 const&, A2 const&);

  RA1A2Functor(FP fp, A1 const& a1, A2 const& a2) : _fp{fp}, _a1{a1}, _a2{a2} {}

  R invoke() override final { return _fp(_a1, _a2); }

 private:
  FP _fp;
  A1 const& _a1;
  A2 const& _a2;
};

template <typename R, typename A1, typename R2>
class RA1R2Functor : public Functor<R> {
 public:
  typedef R (*FP)(A1 const&, R2&);

  RA1R2Functor(FP fp, A1 const& a1, R2& r2) : _fp{fp}, _a1{a1}, _r2{r2} {}

  R invoke() override final { return _fp(_a1, _r2); }

 private:
  FP _fp;
  A1 const& _a1;
  R2& _r2;
};

template <typename A1, typename R2>
class VA1R2Functor : public Functor<void> {
 public:
  typedef void (*FP)(A1 const&, R2&);

  VA1R2Functor(FP fp, A1 const& a1, R2& r2) : _fp{fp}, _a1{a1}, _r2{r2} {}

  void invoke() override final { return _fp(_a1, _r2); }

 private:
  FP _fp;
  A1 const& _a1;
  R2& _r2;
};

}  // namespace dispatch
}  // namespace configuration
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_OPERATION_DISPATCH_H_ */
