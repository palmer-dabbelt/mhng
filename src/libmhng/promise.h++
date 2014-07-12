
/*
 * Copyright (C) 2014 Palmer Dabbelt
 *   <palmer@dabbelt.com>
 *
 * This file is part of mhng.
 *
 * mhng is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mhng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with mhng.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MHNG__PROMISE_HXX
#define MHNG__PROMISE_HXX

#include <functional>
#include <memory>

namespace mhng {
    /* Stores a promise of a value.  The idea here is that I quite
     * frequently want to have access to some value in the database.
     * For efficiency reasons, I don't want to fetch too often.  This
     * means that I want to fetch it 0 times if the value is not used,
     * and exactly 1 time if the value is used (there's not really a
     * caching problem because all the programs run for a very short
     * time).  This class is designed to encapsulate this sort of
     * behavior.  Note that this class is a bit tricky to use:
     * specifically the function that fills out an argument itself
     * takes a pointer to same class.  Essentially what this means is
     * that you're never going to want to return a promise itself but
     * instead actually fill out that promise. */
    template<class A, class T> class promise {
    private:
        std::shared_ptr<T> _ptr;
        std::function<std::shared_ptr<T>(A*)> _func;
        A* _arg;

    public:
        promise(A* arg, std::function<std::shared_ptr<T>(A*)> func)
            : _ptr(NULL),
              _func(func),
              _arg(arg)
            {
            }

        promise(const std::shared_ptr<T>& ptr)
            : _ptr(ptr),
              _func(NULL),
              _arg(NULL)
            {
            }

    public:
        operator std::shared_ptr<T>()
            {
                if (_ptr == NULL)
                    _ptr = _func(_arg);
                return _ptr;
            }
    };
}

#endif
