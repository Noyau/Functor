#include "Functor.h"

/////////////////////////////////////////////////
// FUNCTOR BASE
/////////////////////////////////////////////////
FunctorBase::FunctorBase()
    : FunctorBase(nullptr, nullptr)
{}

FunctorBase::FunctorBase(const void* const callee, const void* const function)
    : m_Function{ function }
    , m_Callee{ callee }
{}

FunctorBase::FunctorBase(const FunctorBase& other)
    : FunctorBase(other.m_Callee, other.m_Function)
{}

FunctorBase::~FunctorBase()
{}

FunctorBase::operator bool() const
{
    return m_Function != nullptr || m_Callee != nullptr;
}