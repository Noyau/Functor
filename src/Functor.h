#pragma once

/////////////////////////////////////////////////
// FUNCTOR BASE
/////////////////////////////////////////////////
class FunctorBase
{
public:
    template<typename Ret, typename... Args>
    using Function = Ret(*)(Args...);

    template<class CallType, typename Ret, typename... Args>
    using Method = Ret(CallType::*)(Args...);

    FunctorBase();
    FunctorBase(const void* const callee, const void* const function);
    FunctorBase(const FunctorBase& other);

    virtual ~FunctorBase();

    operator bool() const;

    union
    {
        const void* const m_Function;
        const void* const m_Method;
    };

    const void* const m_Callee;
};

/////////////////////////////////////////////////
// FUNCTOR <template> (declaration)
/////////////////////////////////////////////////
template<typename Ret, typename... Args>
class Functor : public FunctorBase
{
public:
    class DummyInit {};

    Functor(const DummyInit* = nullptr) {}

    virtual ~Functor() {}

    Ret operator()(Args... args) const;

protected:
    using Thunk = FunctorBase::Function<Ret, const FunctorBase&, Args...>;

    Functor(Thunk thunk, const void* const callee, const void* const function);

private:
    Thunk m_Thunk;
};

/////////////////////////////////////////////////
// COMMON FUNCTORS
/////////////////////////////////////////////////

///////////////////////////////////////
// FUNCTORS (without return)
using Functor0 = Functor<void>;
template <typename Arg1>
using Functor1 = Functor<void, Arg1>;
template <typename Arg1, typename Arg2>
using Functor2 = Functor<void, Arg1, Arg2>;
template <typename Arg1, typename Arg2, typename Arg3>
using Functor3 = Functor<void, Arg1, Arg2, Arg3>;
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
using Functor4 = Functor<void, Arg1, Arg2, Arg3, Arg4>;

///////////////////////////////////////
// FUNCTORS (with return)
template <typename Ret>
using Functor0wRet = Functor<Ret>;
template <typename Ret, typename Arg1>
using Functor1wRet = Functor<Ret, Arg1>;
template <typename Ret, typename Arg1, typename Arg2>
using Functor2wRet = Functor<Ret, Arg1, Arg2>;
template <typename Ret, typename Arg1, typename Arg2, typename Arg3>
using Functor3wRet = Functor<Ret, Arg1, Arg2, Arg3>;
template <typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
using Functor4wRet = Functor<Ret, Arg1, Arg2, Arg3, Arg4>;

///////////////////////////////////////
// PREDICATES
template <typename... Args>
using Predicate = Functor<bool, Args...>;
using Predicate0 = Predicate<void>;
template <typename Ret, typename Arg1>
using Predicate1 = Predicate<Arg1>;
template <typename Ret, typename Arg1, typename Arg2>
using Predicate2 = Predicate<Arg1, Arg2>;
template <typename Ret, typename Arg1, typename Arg2, typename Arg3>
using Predicate3 = Predicate<Arg1, Arg2, Arg3>;
template <typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
using Predicate4 = Predicate<Arg1, Arg2, Arg3, Arg4>;

/////////////////////////////////////////////////
// TRANSLATORS
/////////////////////////////////////////////////

///////////////////////////////////////
// FUNCTION TRANSLATOR <template> (declaration)
template<class FunctionType, typename Ret, typename... Args>
class FunctionTranslator : public Functor<Ret, Args...>
{
    using super = Functor<Ret, Args...>;

public:
    static Ret Thunk(const FunctorBase& ftor, Args... args);

    FunctionTranslator(const FunctionType& func);
};

///////////////////////////////////////
// METHOD TRANSLATOR <template> (declaration)
template<class MethodType, class Callee, typename Ret, typename... Args>
class MethodTranslator : public Functor<Ret, Args...>
{
    using super = Functor<Ret, Args...>;

public:
    static Ret Thunk(const FunctorBase& ftor, Args... args);

    MethodTranslator(const Callee& callee, const MethodType& method);
};

/////////////////////////////////////////////////
// HELPERS
/////////////////////////////////////////////////
namespace FunctorHelper
{
///////////////////////////////////////
// FUNCTION
namespace Function
{
template<typename Ret, typename... Args>
using Type = FunctorBase::Function<Ret, Args...>;

template<typename Ret, typename... Args>
using Translator = FunctionTranslator<Type<Ret, Args...>, Ret, Args...>;
}

///////////////////////////////////////
// MAKE FUNCTOR <template> (declaration) [function]
template<typename Ret, typename... Args>
Function::Translator<Ret, Args...>
MakeFunctor(const Function::Type<Ret, Args...>& func);

///////////////////////////////////////
// METHOD
namespace Method
{
template<class CallType, typename Ret, typename... Args>
using Type = FunctorBase::Method<CallType, Ret, Args...>;

template<class Callee, class CallType, typename Ret, typename... Args>
using Translator = MethodTranslator<Type<CallType, Ret, Args...>, Callee, Ret, Args...>;
}

///////////////////////////////////////
// MAKE FUNCTOR <template> (declaration) [method]
template<class Callee, class CallType, typename Ret, typename... Args>
Method::Translator<Callee, CallType, Ret, Args...>
MakeFunctor(const Callee& callee, const Method::Type<CallType, Ret, Args...>& method);
} // namespace FunctorHelper

/////////////////////////////////////////////////
// MAKE FUNCTOR <template> (declaration)
/////////////////////////////////////////////////
template<typename Ret, typename... Args>
FunctorHelper::Function::Translator<Ret, Args...>
MakeFunctor(const FunctorHelper::Function::Type<Ret, Args...>& func);

template<class Callee, class CallType, typename Ret, typename... Args>
FunctorHelper::Method::Translator<Callee, CallType, Ret, Args...>
MakeFunctor(const Callee& callee, const FunctorHelper::Method::Type<CallType, Ret, Args...>& method);

/////////////////////////////////////////////////
// FUNCTOR <template> (definition)
/////////////////////////////////////////////////
template<typename Ret, typename... Args>
Functor<Ret, Args...>::Functor(Thunk thunk, const void* const callee, const void* const function)
    : FunctorBase(callee, function)
    , m_Thunk{ thunk }
{}

template<typename Ret, typename... Args>
Ret Functor<Ret, Args...>::operator()(Args ...args) const
{
    return m_Thunk(*this, args...);
}

/////////////////////////////////////////////////
// FUNCTION TRANSLATOR <template> (definition)
/////////////////////////////////////////////////
template<class FunctionType, typename Ret, typename... Args>
Ret FunctionTranslator<FunctionType, Ret, Args...>::Thunk(const FunctorBase& ftor, Args ...args)
{
    return (FunctionType(ftor.m_Function))(args...);
}

template<class FunctionType, typename Ret, typename... Args>
FunctionTranslator<FunctionType, Ret, Args...>::FunctionTranslator(const FunctionType& func)
    : super(&Thunk, nullptr, func)
{}

/////////////////////////////////////////////////
// METHOD TRANSLATOR <template> (definition)
/////////////////////////////////////////////////
template<class MethodType, class Callee, typename Ret, typename... Args>
Ret MethodTranslator<MethodType, Callee, Ret, Args...>::Thunk(const FunctorBase& ftor, Args ...args)
{
    Callee* callee{ const_cast<Callee*>(static_cast<const Callee*>(ftor.m_Callee)) };
    MethodType& method{ *const_cast<MethodType*>(static_cast<const MethodType*>(ftor.m_Method)) };
    return (callee->*method)(args...);
}

template<class MethodType, class Callee, typename Ret, typename... Args>
MethodTranslator<MethodType, Callee, Ret, Args...>::MethodTranslator(const Callee& callee, const MethodType& method)
    : super(&Thunk, &callee, &method)
{}

/////////////////////////////////////////////////
// HELPERS (definition)
/////////////////////////////////////////////////
namespace FunctorHelper
{
///////////////////////////////////////
// MAKE FUNCTOR <template> (definition) [function]
template<typename Ret, typename... Args>
Function::Translator<Ret, Args...>
MakeFunctor(const Function::Type<Ret, Args...>& func)
{
    return Function::Translator<Ret, Args...>(func);
}

///////////////////////////////////////
// MAKE FUNCTOR <template> (definition) [method]
template<class Callee, class CallType, typename Ret, typename... Args>
Method::Translator<Callee, CallType, Ret, Args...>
MakeFunctor(const Callee& callee, const Method::Type<CallType, Ret, Args...>& method)
{
    return Method::Translator<Callee, CallType, Ret, Args...>(callee, method);
}
} // namespace FunctorHelper

/////////////////////////////////////////////////
// MAKE FUNCTOR <template> (definition) [function]
/////////////////////////////////////////////////
template<typename Ret, typename... Args>
FunctorHelper::Function::Translator<Ret, Args...>
MakeFunctor(const FunctorHelper::Function::Type<Ret, Args...>& func)
{
    return FunctorHelper::MakeFunctor<Ret, Args...>(func);
}

/////////////////////////////////////////////////
// MAKE FUNCTOR <template> (definition) [method]
/////////////////////////////////////////////////
template<class Callee, class CallType, typename Ret, typename... Args>
FunctorHelper::Method::Translator<Callee, CallType, Ret, Args...>
MakeFunctor(const Callee& callee, const FunctorHelper::Method::Type<CallType, Ret, Args...>& method)
{
    return FunctorHelper::MakeFunctor<Callee, CallType, Ret, Args...>(callee, method);
}
