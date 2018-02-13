#pragma once

template <typename T>
struct ValueComparatorPredicate
{
    T value;

    ValueComparatorPredicate(const T value) 
        : value(value) 
    {}

    bool operator()(const T other) const 
    { 
        return other == value; 
    }
};