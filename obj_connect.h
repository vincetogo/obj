//
// signal.h
// Passageways
//
// Created by Vince Tourangeau on 2013-11-26.
// Copyright (c) 2014 Vincent Tourangeau.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
