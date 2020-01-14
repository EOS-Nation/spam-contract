#!/usr/bin/env bash

mkdir -p dist

# spam
eosio-cpp -abigen \
  spam.cpp \
  -contract spam \
  -o ./dist/spam.wasm \
