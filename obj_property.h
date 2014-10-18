//
// property.h
//
// Created by Vince Tourangeau on 2014-05-13.
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

#ifndef __OBJ_PROPERTY_H__
#define __OBJ_PROPERTY_H__

#include <obj_signal.h>

namespace obj
{
    class connection;
    
    template<typename T> class signal;
    
    // return type
    
    enum class var_return_type
    {
        copy,
        ref
    };

    template<typename T, var_return_type V>
    struct return_type;
    
    template<typename T>
    struct return_type<T, var_return_type::copy>
    {
        using type = T;
    };
    
    template<typename T>
    struct return_type<T, var_return_type::ref>
    {
        using type = T&;
    };
    
    template<typename T, var_return_type V>
    struct const_return_type;
    
    template<typename T>
    struct const_return_type<T, var_return_type::copy>
    {
        using type = T;
    };
    
    template<typename T>
    struct const_return_type<T, var_return_type::ref>
    {
        using type = const T&;
    };
    
    enum class indirection_type
    {
        same,
        derefrenced
    };
    
    template<typename T>
    struct compare
    {
        static bool equal(const T& lhs, const T& rhs)
        {
            return lhs == rhs;
        }
    };
    
    template<typename T>
    struct compare<std::weak_ptr<T>>
    {
        static bool equal(const std::weak_ptr<T>& lhs,
                          const std::weak_ptr<T>& rhs)
        {
            return lhs.lock() == rhs.lock();
        }
    };
    
    template<typename T>
    struct compare<std::function<T>>
    {
        static bool equal(const std::function<T>& lhs,
                          const std::function<T>& rhs)
        {
            return false;
        }
    };

    template<class T, var_return_type V>
    class basic_property_base
    {
    
    public:
        using ReturnT = typename return_type<T,V>::type;
        using ConstReturnT = typename const_return_type<T,V>::type;
        
        basic_property_base() :
            _val()
        {
        }
        
        basic_property_base(const T& val) :
            _val(val)
        {
        }
        
        basic_property_base(const basic_property_base& other) :
            _val(other._val)
        {
        }
        
        operator ReturnT()
        {
            return _val;
        }
        
        operator ConstReturnT() const
        {
            return _val;
        }
        
        ReturnT operator()()
        {
            return _val;
        }
        
        ConstReturnT operator()() const
        {
            return _val;
        }

    protected:
        T   _val;

    };
    
    template<class T, class D, template<class> class S, class C>
    class dynamic_signaller
    {
        friend D;
        
    public:
        
        C
        connect(const std::function<void(const T&)>& fn)
        {
            return _changedSig.connect(fn);
        }
        
        template<class... Args>
        C
        connect(const std::function<void(const T&)>& fn, Args... args)
        {
            return _changedSig.connect(fn, args...);
        }
        
        C
        connect(const std::function<void(const T&, const T&)>& fn)
        {
            return _changedSig2.connect(fn);
        }
        
        template<class... Args>
        C
        connect(const std::function<void(const T&, const T&)>& fn, Args... args)
        {
            return _changedSig2.connect(fn, args...);
        }
        
        void disconnect()
        {
            _changedSig.disconnect();
            _changedSig2.disconnect();
        }
        
    private:
        
        void send(const T& newVal)
        {
            _changedSig(newVal);
        }
        
        void send(const T& newVal, const T& oldVal)
        {
            _changedSig2(newVal, oldVal);
        }
        
    private:
        
        S<void(const T&)>           _changedSig;
        S<void(const T&, const T&)> _changedSig2;
    };
    
    
    template<class T, class D, var_return_type V>
    class dynamic_property_base
    {
    public:
        
        using ReturnT = typename return_type<T,V>::type;
        using ConstReturnT = typename const_return_type<T,V>::type;
        
        dynamic_property_base(D* dataObj,
                              ConstReturnT(D::*getter)() const) :
            _dataObj(dataObj),
            _getter(getter)
        {
        }
        
        operator ReturnT()
        {
            return (_dataObj->*_getter)();
        }
        
        operator ConstReturnT() const
        {
            return (_dataObj->*_getter)();
        }
        
        ReturnT operator()()
        {
            return (_dataObj->*_getter)();
        }
        
        ConstReturnT operator()() const
        {
            return (_dataObj->*_getter)();
        }
        
    protected:
        dynamic_property_base(const dynamic_property_base&);
        
        D*  _dataObj;
        
        ConstReturnT(D::*_getter)() const;
    };
    
    // const properties
    template<class T, var_return_type V>
    class const_basic_property : public basic_property_base<T,V>
    {
        
    public:
        const_basic_property(const T& val) :
        basic_property_base<T,V>(val)
        {
        }
        
        const_basic_property(const const_basic_property& other) :
        basic_property_base<T,V>(other)
        {
        }
        
    private:
        
        const_basic_property<T,V>&
        operator=(const T& rhs);
    };
    
    
    template<class T, class D, var_return_type V, template<class> class S, class C>
    class const_basic_dynamic_property : public dynamic_property_base<T,D,V>,
                                         public dynamic_signaller<T,D,S,C>
    {
        
    public:
        using ConstReturnT =
            typename const_basic_dynamic_property::ConstReturnT;
        
        const_basic_dynamic_property(D* dataObj,
                                     ConstReturnT(D::*getter)() const) :
            dynamic_property_base<T,D,V>(dataObj, getter)
        {
        }
        
        const_basic_dynamic_property(const const_basic_dynamic_property& other) :
            dynamic_property_base<T,D,V>(other)
        {
        }
        
    private:
        
        const_basic_dynamic_property<T,D,V,S,C>&
        operator=(const T& rhs);
    };
    
    
    // mutable properties
    
    template<typename T, var_return_type V, template<class> class S, class C>
    class basic_property : public basic_property_base<T,V>
    {
    
    public:
        basic_property() :
            basic_property_base<T,V>()
        {
        }
        
        basic_property(const T& val) :
            basic_property_base<T,V>(val)
        {
        }
        
        basic_property(const basic_property& other) :
            basic_property_base<T, V>(other)
        {
        }
        
        basic_property<T,V,S,C>&
        operator=(const T& rhs)
        {
            if (!compare<T>::equal(this->_val, rhs))
            {
                T* oldVal = nullptr;
                
                if (this->_changedSig2.connected())
                {
                    oldVal = new T(this->_val);
                }
                
                this->_val = rhs;
                this->_changedSig(this->_val);
                
                if (oldVal)
                {
                    this->_changedSig2(this->_val, *oldVal);
                    delete oldVal;
                }
            }
            
            return *this;
        }
        
        C
        connect(const std::function<void(const T&)>& fn)
        {
            return _changedSig.connect(fn);
        }
        
        template<class... Args>
        C
        connect(const std::function<void(const T&)>& fn, Args... args)
        {
            return _changedSig.connect(fn, args...);
        }
        
        C
        connect(const std::function<void(const T&, const T&)>& fn)
        {
            return _changedSig2.connect(fn);
        }
        
        template<class... Args>
        C
        connect(const std::function<void(const T&, const T&)>& fn, Args... args)
        {
            return _changedSig2.connect(fn, args...);
        }
        
        void disconnect_all()
        {
            _changedSig.disconnect_all();
            _changedSig2.disconnect_all();
        }

    protected:
        S<void(const T&)>           _changedSig;
        S<void(const T&, const T&)> _changedSig2;
    };
    
    template<class T, class D, var_return_type V,
              template<class> class S, class C>
    class basic_dynamic_property :
        public dynamic_property_base<T,D,V>,
        public dynamic_signaller<T, D, S, C>
    {
    public:
        using ConstReturnT =
            typename dynamic_property_base<T,D,V>::ConstReturnT;
        
        basic_dynamic_property(D* dataObj,
                               ConstReturnT(D::*getter)() const,
                               void(D::*setter)(const T&)) :
            dynamic_property_base<T,D,V>(dataObj, getter),
            _setter(setter)
        {
        }
        
        basic_dynamic_property<T,D,V,S,C>&
        operator=(const T& rhs)
        {
            (dynamic_property_base<T,D,V>::_dataObj->*_setter)(rhs);
            
            return *this;
        }
        
    private:
        basic_dynamic_property(const basic_dynamic_property&);
        
        void(D::*_setter)(const T&);
    };
    
    template<typename T> using property =
        basic_property<T, var_return_type::copy, obj::signal, obj::connection>;
    template<typename T> using ref_property =
        basic_property<T, var_return_type::ref, obj::signal, obj::connection>;
    
    template<typename T> using const_property =
        const_basic_property<T, var_return_type::copy>;
    template<typename T> using const_ref_property =
        const_basic_property<T, var_return_type::ref>;
    
    template<typename T, typename D> using dynamic_property =
        basic_dynamic_property<T, D, var_return_type::copy,
                               obj::signal, obj::connection>;
    template<typename T, typename D> using dynamic_ref_property =
        basic_dynamic_property<T, D, var_return_type::ref,
                               obj::signal, obj::connection>;

    template<typename T, typename D> using const_dynamic_property =
        const_basic_dynamic_property<T, D, var_return_type::copy, obj::signal, obj::connection>;
    template<typename T, typename D> using const_dynamic_ref_property =
        const_basic_dynamic_property<T, D, var_return_type::ref, obj::signal, obj::connection>;
}

#endif
