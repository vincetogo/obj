//
//  obj_algorithm.h
//  Passageways
//
//  Created by Vince Tourangeau on 1/4/15.
//  Copyright (c) 2015 Vincent Tourangeau. All rights reserved.
//

#ifndef __OBJ_ALGORITHM_H__
#define __OBJ_ALGORITHM_H__

#include <algorithm>

#include <assert.h>

namespace obj
{
    template<class C, class InputIterator, class OutputIterator>
    C
    set_operation(const C& a, const C& b,
                  OutputIterator (*operation)(InputIterator,
                                              InputIterator,
                                              InputIterator,
                                              InputIterator,
                                              OutputIterator) )
    {
        C result;
        std::insert_iterator<C> insert(result, result.begin());
        
        operation(a.begin(), a.end(), b.begin(), b.end(), insert);
        
        return result;
    };
    
    template<class C>
    C
    set_intersection(const C& a, const C& b)
    {
        using InputIterator = decltype(a.begin());
        using OutputIterator = std::insert_iterator<C>;
        
        return set_operation(a, b, std::set_intersection<InputIterator, InputIterator, OutputIterator>);
    }
    
    template<class C>
    C
    set_difference(const C& a, const C& b)
    {
        using InputIterator = decltype(a.begin());
        using OutputIterator = std::insert_iterator<C>;
        
        return set_operation(a, b, std::set_difference<InputIterator, InputIterator, OutputIterator>);
    }
    
    template<class C>
    C
    set_union(const C& a, const C& b)
    {
        using InputIterator = decltype(a.begin());
        using OutputIterator = std::insert_iterator<C>;
        
        return set_operation(a, b, std::set_union<InputIterator, InputIterator, OutputIterator>);
    }

    template<class C>
    C copy(const C& source)
    {
        C result;
        
        std::insert_iterator<C> insert(result, result.begin());
        
        copy(source.begin(), source.end(), insert);
        
        return result;
    }

    template<class C, class Pred>
    C copy_if(const C& source, Pred pred)
    {
        C result;
        
        std::insert_iterator<C> insert(result, result.begin());
        
        copy_if(source.begin(), source.end(), insert, pred);
        
        return result;
    }

    template <class C, class T>
    typename C::const_iterator find(const C& c, const T& val)
    {
        return std::find(c.begin(), c.end(), val);
    }
    
    template <class C, class T>
    bool contains(const C& c, const T& val)
    {
        return find(c, val) != c.end();
    }

    template <class C, class Pred>
    typename C::iterator find_if(const C& c, Pred pred)
    {
        return std::find_if(c.begin(), c.end(), pred);
    }
    
    template<class C, class Fn>
    void for_each(C& c, Fn fn)
    {
        std::for_each(c.begin(), c.end(), fn);
    }
        
    template<class C1, class C2>
    typename C1::iterator find_first_of(const C1& c1, const C2& c2)
    {
        return std::find_first_of(c1.begin(), c1.end(), c2.begin(), c2.end());
    }
    
    template<class C1, class C2, class Fn>
    C1 transform(const C2& source, Fn fn);
    
    template<class M>
    std::set<typename M::mapped_type> range(const M& source)
    {
        using result_type = std::set<typename M::mapped_type>;
        result_type result;
        
        auto get_val =
        [](const typename M::value_type& key_value) -> typename M::mapped_type
        {
            return key_value.second;
        };
        
        return transform<result_type>(source, get_val);
    };
    
    template<class C, class Pred>
    void remove_if(C& c, Pred pred)
    {
        auto newEnd = std::remove_if(c.begin(), c.end(), pred);
        c.erase(newEnd, c.end());
    }
    
    template<class C1, class C2, class Fn>
    C1 transform(const C2& source, Fn fn)
    {
        C1 result;
        std::insert_iterator<C1> insert(result, result.begin());
        
        std::transform(source.begin(), source.end(), insert, fn);
        
        return result;
    }

    template<class C1, class C2, class Fn>
    C1 transform_and_copy(const C2& source, Fn fn)
    {
        return copy(transform<C1>(source, fn));
    }

    template<class C1, class C2, class Fn, class Pred>
    C1 transform_and_copy_if(const C2& source, Fn fn, Pred pred)
    {
        return copy_if(transform<C1>(source, fn), pred);
    }
    
#ifndef DEBUG
#define assert_for_each(a,b)
#else
    template<class C, class Pred>
    void assert_for_each(C& c, Pred pred)
    {
        for (const auto& value : c)
        {
            assert(pred(value));
        }
    }
#endif
}

#endif
