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

#ifndef __OBJ_SIGNAL_H__
#define __OBJ_SIGNAL_H__

#include <assert.h>

#include <functional>
#include <map>
#include <memory>
#include <set>

namespace obj
{
    class connection;
    
    class signal_base
    {
    public:
        
        virtual void disconnect(const connection& cnxn) = 0;
        
    };
    
    class connection
    {
    
    private:
        class details
        {
        public:
            
            details(int idx, void* function, signal_base* signal, bool fireOnce) :
                _idx(idx),
                _fireOnce(fireOnce),
                _function(function),
                _signal(signal),
                _valid(idx >= 0)
            {}
            
            details(const details& rhs) :
                _idx(rhs._idx),
                _fireOnce(rhs._fireOnce),
                _function(rhs._function),
                _signal(rhs._signal),
                _valid(rhs._valid)
            {}
            
            bool            _fireOnce;
            void*           _function;
            int             _idx;
            signal_base*    _signal;
            bool            _valid;
        };
        
    public:
        connection() :
            _details()
        {}
        
        connection(int idx, void* function, signal_base* signal, bool fireOnce) :
            _details(new details(idx, function, signal, fireOnce))
        {}
        
        connection(const connection& rhs) :
            _details(rhs._details)
        {}
        
        connection& operator=(const connection& rhs)
        {
            _details = rhs._details;
            
            return *this;
        }
        
        void disconnect() const
        {
            if (valid())
            {
                _details->_valid = false;
                
                if (_details->_signal)
                {
                    _details->_signal->disconnect(*this);
                }
            }
        }
        
        bool operator==(const connection& rhs) const
        {
            return index() == rhs.index();
        }
        
        bool operator<(const connection& rhs) const
        {
            return index() < rhs.index();
        }
        
        template<typename slot>
        const slot& function() const
        {
            return *reinterpret_cast<slot*>(_details->_function);
        }
        
        bool fireOnce() const
        {
            return _details ? _details->_fireOnce : false;
        }
        
        int index() const
        {
            return _details ? _details->_idx : -1;
        }
        
        bool valid() const
        {
            return _details ? _details->_valid : false;
        }
        
        template<typename slot>
        void clear() const
        {
            delete(reinterpret_cast<slot*>(_details->_function));
            
            _details->_function = 0;
        }
        
        std::shared_ptr<details>    _details;
    };
    
    
    class observer
    {
        friend class signal_base;
        
    public:
        virtual ~observer()
        {
            for (const auto& cnxn : _obj_signal_connections )
            {
                cnxn.disconnect();
            }
        }
        
        void add_connection(const connection& cnxn)
        {
            _obj_signal_connections.insert(cnxn);
        }

    private:
        
        std::set<connection> _obj_signal_connections;
    };
    
    
    template<typename T>
    class signal_common;
    
    template<typename ReturnType, typename... ArgTypes>
    class signal_common<ReturnType(ArgTypes...)> : public signal_base
    {
    public:
        typedef std::function<ReturnType(ArgTypes...)> slot;
        
        signal_common() :
            _calling(false),
            _slots()
        {
        }
        
        virtual ~signal_common()
        {
            disconnect_all();
        }
        
        connection connect(const slot& fn, bool fireOnce = false)
        {
            clean();
            
            int idx = 0;
            
            if (!_slots.empty())
            {
                idx = _slots.rbegin()->index()+1;
            }
            
            slot* fnCopy = new slot(fn);

            connection result(idx, fnCopy, this, fireOnce);
            
            assert(_slots.find(result) == _slots.end());
            
            _slots.insert(result);
            
            assert(*_slots.rbegin() == result);
            
            return result;
        }
        
        template<typename T>
        connection connect(const slot& fn, T& hostObj, bool fireOnce = false)
        {
            connection result = connect(fn, fireOnce);
            hostObj.add_connection(result);
            
            return result;
        }
        
        template<typename T>
        connection connect(const slot& fn, T* hostObj, bool fireOnce = false)
        {
            connection result = connect(fn, fireOnce);
            hostObj->add_connection(result);
            
            return result;
        }
        
        template<typename T>
        connection connect(const slot& fn, std::shared_ptr<T>& hostObj, bool fireOnce = false)
        {
            connection result = connect(fn, fireOnce);
            hostObj.add_connection(result);
            
            return result;
        }
        
        bool connected() const
        {
            return !_slots.empty();
        }
        
        void disconnect(const connection& cnxn)
        {
            assert(cnxn.index() >= 0);
            assert(!cnxn.valid());
            
            clean();
        }
        
        void disconnect_all()
        {
            std::set<connection> cnxns;
            
            for (const auto& cnxn : _slots)
            {
                cnxns.insert(cnxn);
            }
            
            for (auto& cnxn : cnxns)
            {
                cnxn.disconnect();
            }
        }
        
    protected:
        void clean()
        {
            if (_calling)
            {
                return;
            }
            
            std::set<connection> invalidCnxns;
            
            for (const auto& cnxn : _slots)
            {
                if (!cnxn.valid())
                {
                    invalidCnxns.insert(cnxn);
                }
            }
            
            for (const auto& cnxn : invalidCnxns)
            {
                assert(_slots.find(cnxn) != _slots.end());
                
                _slots.erase(cnxn);
                cnxn.template clear<slot>();
            }
        }
        
    protected:
        mutable bool            _calling;
        
        std::set<connection>    _slots;
    };
    
    template<typename T>
    class signal
    {
    };
    
    template<typename... ArgTypes>
    class signal<void(ArgTypes...)> : public signal_common<void(ArgTypes...)>
    {
        typedef std::function<void(ArgTypes...)>    slot;
        typedef signal_common<void(ArgTypes...)>    base_class;
    
    public:
        void operator()(ArgTypes... args) const
        {
            bool oldCalling = base_class::_calling;
            
            base_class::_calling = true;
            
            for (const auto& cnxn : base_class::_slots)
            {
                if (cnxn.valid())
                {
                    cnxn.template function<slot>()(args...);
                    
                    if (cnxn.fireOnce())
                    {
                        cnxn.disconnect();
                    }
                }
            }
            
            base_class::_calling = oldCalling;
            
            const_cast<signal*>(this)->clean();
        }
    };
    
    template<typename ReturnType, typename... ArgTypes>
    class signal<ReturnType(ArgTypes...)> : public signal_common<ReturnType(ArgTypes...)>
    {
        typedef std::function<ReturnType(ArgTypes...)>  slot;
        typedef signal_common<ReturnType(ArgTypes...)>  base_class;
        
    public:
        ReturnType operator()(ArgTypes... args) const
        {
            ReturnType result;
            bool oldCalling = base_class::_calling;
            
            base_class::_calling = true;
            
            for (const auto& cnxn : base_class::_slots)
            {
                if (cnxn.valid())
                {
                    result = cnxn.template function<slot>()(args...);
                }
            }
            
            base_class::_calling = oldCalling;
            
            const_cast<signal*>(this)->clean();
            
            return result;
        }
    };
}

#endif
