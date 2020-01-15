#!/usr/bin/env bash

cleos -v push action spam init '["2020-01-14T18:10:00", "add"]' -p spam
cleos -v push action spam init '["2020-01-14T18:10:00", "log"]' -p spam