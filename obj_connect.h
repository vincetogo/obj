//
//  obj_connect.h
//  Passageways
//
//  Created by Vince Tourangeau on 2014-07-02.
//  Copyright (c) 2014 Vincent Tourangeau. All rights reserved.
//

#ifndef __OBJ_CONNECT_H__
#define __OBJ_CONNECT_H__

#include <obj_property.h>

namespace obj
{
    template<class T>
    obj::connection connect(const obj::property<T>& source, obj::property<T>& dest)
    {
        auto onSourceChanged =
        [=](const T& newVal, const T& oldVal)
        {
            dest = newVal;
        };
        
        return source.connect(onSourceChanged);
    };

    template<class S, class D>
    obj::connection connect(obj::property<S>& source, obj::property<D>& dest, std::function<D(const S&)> converter)
    {
        auto onSourceChanged =
        [=, &dest](const S& newVal)
        {
            dest = converter(newVal);
        };
        
        return source.connect(onSourceChanged);
    };
}

#endif
