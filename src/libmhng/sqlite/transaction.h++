
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

#ifndef MHNG__SQLITE__TRANSACTION_HXX
#define MHNG__SQLITE__TRANSACTION_HXX

#include <memory>

namespace mhng {
    namespace sqlite {
        class transaction;
        class deferred_transaction;
        class immediate_transaction;
        class exclusive_transaction;
        typedef std::shared_ptr<transaction> transaction_ptr;
        typedef std::shared_ptr<deferred_transaction> deferred_transaction_ptr;
        typedef std::shared_ptr<immediate_transaction> immediate_transaction_ptr;
        typedef std::shared_ptr<exclusive_transaction> exclusive_transaction_ptr;
    }
}

#include "connection.h++"

namespace mhng {
    namespace sqlite {
        /* This is a generic transaction object.  Note that you'll
         * want to use one of the more specific ones below, as they're
         * actually useful. */
        class transaction {
        public:
            /* Here is an explicitly non-shared pointer to the
             * connection, which we need because these come from the
             * connection and I don't want to deal with a self-pointer
             * right now... */
            connection *_conn;

        public:
            /* Creates a new transaction. */
            transaction(connection *conn);

            /* Closes a transaction, actually committing it. */
            virtual ~transaction(void);
        };

        class exclusive_transaction: public transaction {
        public:
            exclusive_transaction(connection *conn);
            ~exclusive_transaction(void);
        };

        class immediate_transaction: public exclusive_transaction {
        public:
            immediate_transaction(connection *conn);
            ~immediate_transaction(void);
        };

        class deferred_transaction: public immediate_transaction {
        public:
            deferred_transaction(connection *conn);
            ~deferred_transaction(void);
        };
    }
}

#endif
