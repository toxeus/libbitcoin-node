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
#ifndef LIBBITCOIN_NODE_PROTOCOL_BLOCK_SYNC_HPP
#define LIBBITCOIN_NODE_PROTOCOL_BLOCK_SYNC_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <bitcoin/network.hpp>
#include <bitcoin/node/configuration.hpp>
#include <bitcoin/node/define.hpp>

namespace libbitcoin {
namespace node {
        
/**
 * Blocks sync protocol.
 */
class BCN_API protocol_block_sync
  : public network::protocol_timer, public track<protocol_block_sync>
{
public:
    typedef std::shared_ptr<protocol_block_sync> ptr;

    /**
     * Construct a block sync protocol instance.
     * @param[in]  pool          The thread pool used by the protocol.
     * @param[in]  channel       The channel on which to start the protocol.
     * @param[in]  first_height  The height of the first block in hashes.
     * @param[in]  start_height  The height of the first block in sync range.
     * @param[in]  count         The number of blocks to sync.
     * @param[in]  minimum_rate  The minimum sync rate in bytes per second.
     * @param[in]  hashes        The ordered set of block hashes to sync.
     */
    protocol_block_sync(threadpool& pool, network::p2p&,
        network::channel::ptr channel, size_t first_height,
        size_t start_height, size_t count, uint32_t minimum_rate,
        const hash_list& hashes);

    /**
     * Start the protocol.
     * @param[in]  handler  The handler to call upon sync completion.
     */
    void start(count_handler handler);

private:
    size_t next_height() const;
    size_t current_rate() const;
    const hash_digest& next_hash() const;
    message::get_data build_get_data() const;

    void send_get_blocks(event_handler complete);
    void handle_send(const code& ec, event_handler complete);
    void handle_event(const code& ec, event_handler complete);
    void blocks_complete(const code& ec, count_handler handler);
    bool handle_receive(const code& ec, const message::block& message,
        event_handler complete);

    // This is guarded by protocol_timer/deadline contract (exactly one call).
    size_t byte_count_;
    size_t current_second_;

    // This is write-guarded by the block message subscriber strand.
    std::atomic<size_t> index_;

    const size_t count_;
    const size_t first_height_;
    const size_t start_height_;
    const size_t end_height_;
    const uint32_t minimum_rate_;
    const hash_list& hashes_;
};

} // namespace network
} // namespace libbitcoin

#endif