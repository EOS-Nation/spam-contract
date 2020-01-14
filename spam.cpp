#include "spam.hpp"

void spam::init( const time_point_sec timestamp )
{
    check( timestamp > current_time_point(), "timestamp must be in the future" );
    int64_t delay_sec = (timestamp - current_time_point()).to_seconds();

    print("delay_sec:" + to_string(delay_sec ));

    spam::fire_action fire( get_self(), { get_self(), "active"_n });

    for (uint64_t batch = 0; batch <= MAX_BATCH; ++batch) {
        const uint64_t key = calculate_key( timestamp, batch );
        send_deferred( fire.to_action( timestamp, batch ), key, delay_sec );
    }
}

uint64_t spam::calculate_key( const time_point_sec timestamp, const uint64_t batch )
{
    return timestamp.sec_since_epoch() + batch * 10000;
}

void spam::fire( const time_point_sec timestamp, uint64_t batch )
{
    spam::nounce_action nounce( get_self(), { get_self(), "active"_n });

    for (uint64_t unique = 1; unique <= MAX_UNIQUES; ++unique) {
        nounce.send( timestamp, batch, unique );

        const uint64_t key = calculate_key( timestamp, batch ) + unique;
        send_deferred( nounce.to_action( timestamp, batch, unique ), key, 0 );
    }
}

void spam::nounce( const time_point_sec timestamp, uint64_t batch, uint64_t unique )
{
    print( "unique:" + to_string(unique) + "\n");
    print( "batch:" + to_string(batch) + "\n");
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