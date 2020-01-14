#!/usr/bin/env bash

# Create accounts
cleos create account eosio spam EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# add eosio.code
cleos set account permission spam active --add-code

# Deploy smart contracts
cleos set contract spam ./dist spam.wasm spam.abi