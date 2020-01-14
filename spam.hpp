#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/transaction.hpp>
#include <eosio/time.hpp>
#include <eosio/singleton.hpp>


using namespace eosio;
using namespace std;

static constexpr uint64_t MAX_BATCH = 50;
static constexpr uint64_t MAX_UNIQUES = 200;

struct [[eosio::table("counter"), eosio::contract("spam")]] spam_row {
    uint64_t        actions = 0;
};
typedef eosio::singleton< "counter"_n, spam_row> counter_table;

class [[eosio::contract("spam")]] spam : public contract {
public:
    using contract::contract;

    /**
     * Construct a new contract given the contract name
     *
     * @param {name} receiver - The name of this contract
     * @param {name} code - The code name of the action this contract is processing.
     * @param {datastream} ds - The datastream used
     */
    spam( name receiver, name code, eosio::datastream<const char*> ds )
        : contract( receiver, code, ds ),
            _counter( get_self(), get_self().value )
    {}

    [[eosio::action]]
    void init( const time_point_sec timestamp );

    [[eosio::action]]
    void fire( const time_point_sec timestamp, const uint64_t batch );

    [[eosio::action]]
    void nounce( const time_point_sec timestamp, uint64_t batch, uint64_t unique );

    using init_action = eosio::action_wrapper<"init"_n, &spam::init>;
    using fire_action = eosio::action_wrapper<"fire"_n, &spam::fire>;
    using nounce_action = eosio::action_wrapper<"nounce"_n, &spam::nounce>;

private:
    counter_table      _counter;

    void send_deferred( const eosio::action action, const uint64_t key, const unsigned_int delay_sec );
    uint64_t calculate_key( const time_point_sec timestamp, const uint64_t batch );
};
