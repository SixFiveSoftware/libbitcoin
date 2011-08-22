#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

using boost::asio::deadline_timer;
using std::shared_ptr;

class kernel 
  : public threaded_service,
    private boost::noncopyable, 
    public std::enable_shared_from_this<kernel>
{
public:
    kernel();

    void register_network(network_ptr net_comp);
    network_ptr get_network();
    // Callbacks from network component
    void send_failed(channel_handle chandle, message::version message);
    void send_failed(channel_handle chandle, message::verack message);
    void send_failed(channel_handle chandle, message::getaddr message);
    void send_failed(channel_handle chandle, message::inv message);
    void send_failed(channel_handle chandle, message::getdata message);
    void send_failed(channel_handle chandle, message::getblocks message);

    bool recv_message(channel_handle chandle, message::version message);
    bool recv_message(channel_handle chandle, message::verack message);
    bool recv_message(channel_handle chandle, message::addr message);
    bool recv_message(channel_handle chandle, message::inv message);
    bool recv_message(channel_handle chandle, message::block message);

    void handle_connect(channel_handle chandle);

    void register_storage(storage_ptr stor_comp);
    storage_ptr get_storage();

private:
    void reset_inventory_poll();
    void request_inventories(const boost::system::error_code& ec);
    void accept_inventories(std::error_code ec, message::inv_list invs);
    void send_to_random(channel_handle chandle,
            message::getdata request_message);

    network_ptr network_component_;
    storage_ptr storage_component_;

    shared_ptr<deadline_timer> poll_invs_timeout_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif
