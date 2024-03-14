cmake_minimum_required(VERSION 3.25)

project(bare_demo CXX)

include(bare)

add_bare_module(bare_demo)

target_sources(
  ${bare_demo}
  PRIVATE
    binding.cpp
)
