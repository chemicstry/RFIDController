#include "Desfire.h"

#include "PCH.h"
#include "Utils.h"

namespace asio = websocketpp::lib::asio;

using namespace std;
using json = nlohmann::json;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
typedef websocketpp::client<websocketpp::config::asio_client> client;

typedef std::shared_ptr<Desfire> DesfirePtr;

client c;
client::connection_ptr con;
std::string uri = "ws://TxDoorNx-chemicstry629195.codeanyapp.com:8080";

boost::asio::io_service io;

uint8_t key_data_null[8]  = { 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00 };
uint8_t key_data_new[16]  = { 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00 };
// 00 00 26 00 03 10 00 00 00 00 26 00 03 10 00 00

std::vector<DesfirePtr> Tags;

void on_close(client* c, websocketpp::connection_hdl hdl) {
    c->connect(con);
}

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    
    json data = json::parse(msg->get_payload());
    
    if (data["method"] == "InitiateAuthACK")
    {
        if (!Tags.size())
        {
            std::cout << "No tags" << std::endl;
            return;
        }
        
        DesfirePtr desfire = Tags[0];
        
        BinaryData token = data["params"];
        PrintBin(token);
        
        BinaryData res = desfire->ContinueAuth(token);
        PrintBin(res);
        
        json j;
        j["method"] = "ContinueAuth";
        j["params"] = res;
        c->send(con, j.dump(), websocketpp::frame::opcode::text);
    }

    websocketpp::lib::error_code ec;
    //c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }
}

#define NFC_POLL_INTERVAL 3000

nfc_device *device = NULL;
boost::asio::deadline_timer nfc_timer(io, boost::posix_time::milliseconds(NFC_POLL_INTERVAL));

void nfc_poll(const boost::system::error_code& /*e*/)
{
    FreefareTag *tags = NULL;

    tags = freefare_get_tags (device);
    
    if (!tags) {
        nfc_close (device);
        errx (EXIT_FAILURE, "Error listing tags.");
    }

    for (int i = 0; tags[i]; i++)
    {
        try {
            DesfirePtr desfire(new Desfire(tags[i]));
            Tags.push_back(desfire);
            
            std::string UID = desfire->GetUID();
            
            cout << "UID: " << UID << endl;
            
            Tags.push_back(desfire);
            
            BinaryData RndBE = desfire->InitiateAuth(0, KEY_AES);
            
            json j;
            j["method"] = "InitiateAuth";
            j["params"]["RndBE"] = RndBE;
            j["params"]["UID"] = UID;
            c.send(con, j.dump(), websocketpp::frame::opcode::text);
            
            
        } catch (exception& e) {
            cout << "Exception: " << e.what() << endl;
        }
    }
    
    nfc_timer.expires_from_now(boost::posix_time::milliseconds(NFC_POLL_INTERVAL));
    nfc_timer.async_wait(&nfc_poll);
}

int main()
{
    /*
    // Add a WebSocket echo server on port 9002
    ws_echo_server ws_server;
    ws_server.set_access_channels(websocketpp::log::alevel::all);
    ws_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // The only difference in this code between an internal and external
    // io_service is the different constructor to init_asio
    ws_server.init_asio(&service);

    // Register our message handler
    ws_server.set_message_handler(bind(&on_message,&ws_server,::_1,::_2));
    ws_server.listen(9002);
    ws_server.start_accept();

    // TODO: add a timer?

    // Start the Asio io_service run loop for all
    service.run();
*/
    c.init_asio(&io);
    c.set_message_handler(bind(&on_message,&c,::_1,::_2));
    c.set_close_handler(bind(&on_close,&c,::_1));
    
    c.set_access_channels(websocketpp::log::alevel::none);
    c.set_error_channels(websocketpp::log::elevel::all);
    
    websocketpp::lib::error_code ec;
    con = c.get_connection(uri, ec);
    if (ec) {
        std::cout << "could not create connection because: " << ec.message() << std::endl;
        return 0;
    }

    // Note that connect here only requests a connection. No network messages are
    // exchanged until the event loop starts running in the next line.
    c.connect(con);
    
    int error = EXIT_SUCCESS;
    nfc_context *context;

    nfc_init(&context);
    
    if (context == NULL)
        errx(EXIT_FAILURE, "Unable to init libnfc (malloc)");
    
    nfc_connstring devices[8];
    size_t device_count = nfc_list_devices(context, devices, 8);
    
    if (device_count <= 0)
        errx(EXIT_FAILURE, "No NFC device found.");

    for (size_t d = 0; d < device_count; d++) {
        device = nfc_open(context, devices[d]);
        
        if (!device) {
            warnx("nfc_open() failed.");
            error = EXIT_FAILURE;
            continue;
        } else
            break;
    }
    
    if (!device)
        warnx("No NFC Device opened");
    
    nfc_timer.async_wait(&nfc_poll);
    
    io.run();
    
    nfc_close(device);
    nfc_exit(context);
    
    return 0;
}
