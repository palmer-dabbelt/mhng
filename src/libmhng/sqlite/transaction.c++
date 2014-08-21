
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

#include "transaction.h++"
using namespace mhng;

sqlite::transaction::transaction(sqlite::connection *conn)
    : _conn(conn)
{
}

sqlite::transaction::~transaction(void)
{
    if (_conn == NULL)
        return;

    auto resp = _conn->commit_transaction();
    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        break;
    }
}


sqlite::exclusive_transaction::exclusive_transaction(sqlite::connection *conn)
    : immediate_transaction(conn)
{
}

sqlite::exclusive_transaction::~exclusive_transaction(void)
{
}


sqlite::immediate_transaction::immediate_transaction(sqlite::connection *conn)
    : deferred_transaction(conn)
{
}

sqlite::immediate_transaction::~immediate_transaction(void)
{
}


sqlite::deferred_transaction::deferred_transaction(sqlite::connection *conn)
    : transaction(conn)
{
}

sqlite::deferred_transaction::~deferred_transaction(void)
{
}

