/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__UNKNOWN_HXX
#define MHNG__UNKNOWN_HXX

#include <stdio.h>
#include <stdlib.h>

namespace mhng {
    /* Holds a value that might not be valid. */
    template<class T> class unknown {
    private:
        T _data;
        bool _known;

    public:
        unknown(void)
            : _known(false)
            {
            }

        unknown(const T& data)
            : _data(data),
              _known(true)
            {
            }

    public:
        bool known(void) const
            { return _known; }

        const T& data(void) const
            {
                if (_known == false) {
                    fprintf(stderr, "unknown not known\n");
                    abort();
                }

                return _data;
            }

        T operator=(const T& data)
            {
                _data = data;
                _known = true;
                return data;
            }
    };
}

template<typename T>
bool operator==(const mhng::unknown<T>& a, const T& b)
{
    if (a.known() == false)
        return false;
    return a.data() == b;
}

#endif
