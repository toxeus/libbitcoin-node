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
#ifndef LIBBITCOIN_NODE_SESSION_HEADER_SYNC_HPP
#define LIBBITCOIN_NODE_SESSION_HEADER_SYNC_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <bitcoin/blockchain.hpp>
#include <bitcoin/network.hpp>
#include <bitcoin/node/define.hpp>
#include <bitcoin/node/settings.hpp>
#include <bitcoin/node/utility/header_queue.hpp>

namespace libbitcoin {
namespace node {
    
/// Class to manage initial header download connection, thread safe.
class BCN_API session_header_sync
  : public network::session_batch, track<session_header_sync>
{
public:
    typedef std::shared_ptr<session_header_sync> ptr;

    static code get_first(config::checkpoint& out_first,
        blockchain::block_chain_impl& blockchain);

    static code get_last(config::checkpoint& out_last,
        blockchain::block_chain_impl& blockchain);

    session_header_sync(network::p2p& network, header_queue& hashes,
        blockchain::block_chain_impl& blockchain);

    void start(result_handler handler);

private:
    static config::checkpoint::list sort(config::checkpoint::list checkpoints);

    void handle_started(const code& ec, result_handler handler);
    void new_connection(network::connector::ptr connect,
        result_handler handler);
    void start_syncing(const code& ec, const config::authority& host,
        network::connector::ptr connect, result_handler handler);
    void handle_connect(const code& ec, network::channel::ptr channel,
        network::connector::ptr connect, result_handler handler);
    void handle_complete(const code& ec, network::connector::ptr connect,
        result_handler handler);
    void handle_channel_start(const code& ec, network::connector::ptr connect,
        network::channel::ptr channel, result_handler handler);
    void handle_channel_stop(const code& ec);

    // Thread safe.
    header_queue& hashes_;

    // These do not require guard because they are not used concurrently.
    uint32_t minimum_rate_;
    config::checkpoint last_;
    blockchain::block_chain_impl& blockchain_;
};

} // namespace node
} // namespace libbitcoin

#endif
