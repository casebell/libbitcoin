/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/network/protocol_address.hpp>

#include <functional>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using namespace bc::message;
using std::placeholders::_1;
using std::placeholders::_2;

protocol_address::protocol_address(channel::ptr peer, threadpool& pool,
    hosts& hosts, const network_address& self)
  : hosts_(hosts), self_({ { self } }), protocol_base(peer, pool)
{
    BITCOIN_ASSERT(!self_.addresses.empty());
    if (self_.addresses.front().port != 0)
        ////send(self_, &protocol_address::handle_send_address);

    if (hosts_.capacity() == 0)
        return;

    ////accept<address>(&protocol_address::handle_receive_address, _1, _2);
    ////accept<get_address>(&protocol_address::handle_receive_get_address, _1, _2);
    ////send(get_address(), &protocol_address::handle_send_get_address);
}

std::shared_ptr<protocol_address> protocol_address::shared()
{
    return shared_from_base<protocol_address>();
}

void protocol_address::handle_receive_address(const code& ec,
    const address& message)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure receiving address message from [" << peer() << "] "
            << ec.message();
        stop(error::bad_stream);
        return;
    }

    // Resubscribe to address messages.
    ////accept<address>(&protocol_address::handle_receive_address, _1, _2);

    log_debug(LOG_PROTOCOL)
        << "Storing addresses from [" << peer() << "] ("
        << message.addresses.size() << ")";

    // TODO: manage timestamps (active peers are connected < 3 hours ago).
    hosts_.store(message.addresses, 
        std::bind(&protocol_address::handle_store_addresses,
            shared_from_base<protocol_address>(), _1));
}

void protocol_address::handle_receive_get_address(const code& ec,
    const get_address& message)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure receiving get_address message from [" << peer() << "] "
            << ec.message();
        stop(error::bad_stream);
        return;
    }

    // TODO: pull active hosts from host cache.
    // TODO: need to distort for privacy, don't send currently-connected peers.
    address active;
    if (active.addresses.empty())
        return;

    // TODO: allowing repeated queries can allow a peer to map our history.
    // Resubscribe to get_address messages.
    ////accept<get_address>(&protocol_address::handle_receive_get_address, _1, _2);

    log_debug(LOG_PROTOCOL)
        << "Sending addresses to [" << peer() << "] ("
        << active.addresses.size() << ")";

    ////send(active, &protocol_address::handle_send_address);
}

void protocol_address::handle_send_address(const code& ec) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending address [" << peer() << "] "
            << ec.message();
}

void protocol_address::handle_send_get_address(const code& ec) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending get_address [" << peer() << "] "
            << ec.message();
}

void protocol_address::handle_store_addresses(const code& ec) const
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing addresses from [" << peer() << "] "
            << ec.message();
}

} // namespace network
} // namespace libbitcoin
