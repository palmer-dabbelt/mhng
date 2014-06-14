
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

#include "global_mailrc.h++"

using namespace mh;

static std::string path = "/home/palmer/.local/share/pim/mailrc";
static std::shared_ptr<mailrc> _global_mailrc = NULL;

const std::shared_ptr<const mailrc> mh::global_mailrc(void)
{
    if (_global_mailrc == NULL)
        _global_mailrc = std::make_shared<mailrc>(path);

    return _global_mailrc;
}
