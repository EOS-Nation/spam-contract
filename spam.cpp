#include "spam.hpp"

void spam::init( const time_point_sec timestamp, const name type )
{
    check( type == "add"_n || type == "log"_n, "[type] must be add or log");

    int64_t delay_sec = 0;
    if (timestamp.sec_since_epoch() != 0) {
        delay_sec = (timestamp - current_time_point()).to_seconds();
        check( timestamp > current_time_point(), "timestamp must be in the future" );
    }
    print("{\"delay_sec\":" + to_string(delay_sec) + "}");

    spam::fire_action fire( get_self(), { get_self(), "active"_n });

    for (uint64_t batch = 0; batch <= MAX_BATCH; ++batch) {
        const uint64_t key = calculate_key( timestamp, batch, type ) + delay_sec;
        send_deferred( fire.to_action( timestamp, batch, type ), key, delay_sec );
    }
}

uint64_t spam::calculate_key( const time_point_sec timestamp, const uint64_t batch, const name type )
{
    return timestamp.sec_since_epoch() + batch * 10000 + type.value;
}

void spam::fire( const time_point_sec timestamp, uint64_t batch, const name type )
{
    spam::add_action add( get_self(), { get_self(), "active"_n });
    spam::log_action log( get_self(), { get_self(), "active"_n });

    for (uint64_t unique = 1; unique <= MAX_UNIQUES; ++unique) {
        const uint64_t key = calculate_key( timestamp, batch, type ) + unique + "fire"_n.value;

        if (type == "add"_n) {
            // add.send( timestamp, batch, unique );
            send_deferred( add.to_action( ), key, 0 );
        } else if (type == "log"_n) {
            // log.send( timestamp, batch, unique );
            send_deferred( log.to_action( timestamp, batch, unique ), key, 0 );
        }
    }
}

void spam::add()
{
    auto counter = _counter.get_or_default();
    counter.actions += 1;
    _counter.set( counter, get_self() );
}

void spam::log( const time_point_sec timestamp, uint64_t batch, uint64_t unique )
{
    print("{\"timestamp\":" + to_string(timestamp.sec_since_epoch()),
         ", \"batch:\"" + to_string(batch),
         ", \"unique:\"" + to_string(unique));
}

void spam::send_deferred( const eosio::action action, const uint64_t key, const unsigned_int delay_sec )
{
    eosio::transaction deferred;
    deferred.delay_sec = delay_sec;
    deferred.expiration = current_time_point() + time_point_sec(3600);
    deferred.actions.emplace_back( action );

    // cancel & send
    eosio::cancel_deferred( key );
    deferred.send( key, get_self() );
}