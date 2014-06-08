//
// obj_function.h
// Passageways
//
// Created by Vince Tourangeau on 2014-06-06.
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


#ifndef __OBJ_FUNCTION_H__
#define __OBJ_FUNCTION_H__

#include <functional>

namespace obj
{
    class function_wrapper
    {
        virtual ~function_wrapper() {};
    };
    
    template<typename R, typename... A>
    class typed_function_wrapper : public function_wrapper
    {
    public:
        
        function(std::function<R(A...)> fn) :
            _fn(fn)
        {
        }
        
        R operator()(A... args)
        {
            return _fn(args...);
        }
        
    private:
        
        std::function<R(A...)> _fn;
    };
    
    class function
    {
    public:
        
        template<typename R, typename... A>
        function(std::function<R(A...)> fn) :
            _wrappedFn(new typed_function_wrapper<R, A...>(fn))
        {
            
        }
        
        template<typename R, typename... A>
        R operator()(A... args)
        {
            assert(isA<typed_function_wrapper<R, A...>(_wrappedFn));
            
            if (isA<typed_function_wrapper<R, A...>(_wrappedFn))
            {
                return asA<typed_function_wrapper<R, A...>(_wrappedFn)(args);
            }
            else
            {
                return R();
            }
        }
        
        
    private:
        unique_ptr<function_wrapper> _wrappedFn;
    };
}



#endif
